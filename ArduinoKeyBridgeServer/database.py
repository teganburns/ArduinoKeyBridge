import os
import uuid
from datetime import datetime
from pymongo import MongoClient
from config import *
import logging
import shutil

class dbManager:
    def __init__(self):
        self.client = MongoClient(MONGODB_URI)
        self.db = self.client[DATABASE_NAME]
        self.screenshot= self.db[SCREENSHOT_COLLECTION_NAME]
        self.chatgpt = self.db[CHATGPT_COLLECTION_NAME]
        # Ensure all relevant directories exist
        for d in [SCREENSHOT_ACTIVE_DIR, SCREENSHOT_ARCHIVE_DIR, CHATGPT_ACTIVE_DIR, CHATGPT_ARCHIVE_DIR, DATABASE_DIR]:
            os.makedirs(d, exist_ok=True)
        self.reset_database() # we will do this for now but in prod we will remove 

    def get_collection(self, name):
        return self.db[name]


    def reset_database(self):
        """
        Completely remove all collections and data from the database.
        Also deletes all files in screenshots and chatgpt directories.
        Use with caution!
        """
        # Drop all collections
        for collection_name in self.db.list_collection_names():
            self.db.drop_collection(collection_name)
        logging.warning("All collections dropped. Database has been reset.")

        # List of directories to clear
        dirs_to_clear = [
            SCREENSHOT_ACTIVE_DIR,
            SCREENSHOT_ARCHIVE_DIR,
            CHATGPT_ACTIVE_DIR,
            CHATGPT_ARCHIVE_DIR,
            DATABASE_DIR,
        ]

        # Delete all files in each directory
        for dir_path in dirs_to_clear:
            if os.path.exists(dir_path):
                for filename in os.listdir(dir_path):
                    file_path = os.path.join(dir_path, filename)
                    try:
                        if os.path.isfile(file_path) or os.path.islink(file_path):
                            os.unlink(file_path)
                        elif os.path.isdir(file_path):
                            shutil.rmtree(file_path)
                    except Exception as e:
                        logging.error(f"Failed to delete {file_path}: {e}")

        logging.warning("All files in screenshots and chatgpt directories have been deleted.")

    #####################################
    # Screenshot Database Methods
    # All screenshot-specific database 
    # methods are organized here
    #####################################

    def insert_screenshot(self, filename, relative_path, status='active', timestamp=None, **metadata):
        """
        Insert a new image record. Returns the file_id.
        """
        file_id = str(uuid.uuid4())
        doc = {
            "file_id": file_id,
            "filename": filename,
            "path": relative_path,
            "timestamp": timestamp or datetime.now().isoformat(),
            "status": status,
        }
        doc.update(metadata)
        self.screenshot.insert_one(doc)
        logging.info(f"Inserted screenshot: file_id={file_id}, path={relative_path}")
        return file_id

    def get_last_screenshot(self):
        """Get the last screenshot from the database."""
        logging.info("Getting last screenshot from database.")
        logging.info(f"Last screenshot: {self.screenshot.find_one(sort=[('timestamp', -1)])}")
        return self.screenshot.find_one(sort=[("timestamp", -1)])


    def get_screenshot_by_id(self, file_id):
        """Retrieve an screenshot document by its unique file_id."""
        return self.screenshot.find_one({"file_id": file_id})

    def list_screenshot(self, query=None):
        """List images, optionally filtered by a query dict."""
        return list(self.screenshot.find(query or {}))

    def list_active_screenshot(self):
        """List all screenshot in the active directory."""
        return self.list_screenshot({"status": "active"})

    def list_archive_screenshot(self):
        """List all screenshot in the archive directory."""
        return self.list_screenshot({"status": "archive"})

    def update_screenshot(self, file_id, **updates):
        """Update metadata for an image by file_id."""
        return self.screenshot.update_one({"file_id": file_id}, {"$set": updates})

    def delete_screenshot(self, file_id):
        """Remove an screenshot record by file_id. and delete the file from the directory."""
        doc = self.get_screenshot_by_id(file_id)
        if doc:
            os.remove(self.get_screenshot_absolute_path(doc))
            return self.screenshot.delete_one({"file_id": file_id})
        return False

    def get_screenshot_absolute_path(self, doc):
        """Given a document, return the absolute path to the image file."""
        if doc and "filename" in doc:
            # Use the correct directory based on status
            directory = SCREENSHOT_ACTIVE_DIR if doc.get("status") == "active" else SCREENSHOT_ARCHIVE_DIR
            return os.path.join(directory, doc["filename"])
        return None

    def archive_screenshot(self, file_id):
        """
        Move an image from active to archive, and update its status and path in the database.
        If already archived, do nothing and return False.
        """
        # Get the current document and its path
        doc = self.get_screenshot_by_id(file_id)
        if not doc:
            logging.error(f"No screenshot found with file_id: {file_id}")
            return False
        if doc.get("status") == "archive":
            logging.info(f"Screenshot {file_id} is already archived.")
            return False
        src = os.path.join(SCREENSHOT_ACTIVE_DIR, doc["filename"])
        dst = os.path.join(SCREENSHOT_ARCHIVE_DIR, doc["filename"])
        # Check if destination already exists
        if os.path.exists(dst):
            logging.warning(f"Destination path '{dst}' already exists. Skipping move.")
            # Still update the database status if not already archived
            self.update_screenshot(file_id, status="archive", path=SCREENSHOT_ARCHIVE_DIR)
            return False
        # Move the file
        shutil.move(src, dst)
        # Update the database
        updates = {"status": "archive", "path": SCREENSHOT_ARCHIVE_DIR}
        self.update_screenshot(file_id, **updates)
        logging.info(f"Archive screenshot: {file_id}")
        return True


    def screenshot_exists(self, file_id):
        """Check if an image with a given file_id exists."""
        return self.screenshot.count_documents({"file_id": file_id}) > 0

    def count_screenshots(self, status=None):
        """Count images, optionally filtered by status (active/archive)."""
        query = {"status": status} if status else {}
        return self.screenshot.count_documents(query)

    def get_screenshot_metadata(self, file_id):
        """Retrieve all metadata for a given image."""
        doc = self.get_screenshot_by_id(file_id)
        if doc:
            return {k: v for k, v in doc.items() if k != '_id'}
        return None 
    

    #####################################
    # ChatGPT Methods
    # All ChatGPT-specific database methods
    # are organized in this section
    #####################################


    