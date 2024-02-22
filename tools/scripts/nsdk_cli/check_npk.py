#!/usr/bin/env python

import os
import sys
import yaml
import argparse

def find_files(directory, file_name):
    """
    Recursively search the specified directory and its subdirectories for files with the given name.

    :param directory: The path to the directory to search in.
    :param file_name: The name of the file to find.
    :return: A list of paths to all found files.
    """
    found_files = []
    for root, dirs, files in os.walk(directory):
        if file_name in files:
            found_files.append(os.path.join(root, file_name))
    return found_files

def validate_yaml(file_path):
    """
    Validate a YAML file to check if it is a valid YAML document.

    :param file_path: The path to the YAML file.
    :return: A tuple containing a boolean (True if the file is valid, False otherwise) and an error message (if the file is invalid).
    """
    try:
        with open(file_path, 'r') as file:
            yaml.safe_load(file)
            return True, None
    except yaml.YAMLError as exc:
        return False, exc

def main():
    parser = argparse.ArgumentParser(description="Validate npk.yml files.")
    parser.add_argument("-d", "--directory", required=True, help="The directory to search for npk.yml files.")
    args = parser.parse_args()

    # Check if the directory exists
    if not os.path.isdir(args.directory):
        print("The directory {} does not exist!".format(args.directory))
        sys.exit(1)

    yaml_files = find_files(args.directory, "npk.yml")
    failed_files = []

    for file_path in yaml_files:
        is_valid, error = validate_yaml(file_path)
        if not is_valid:
            print("- {}: FAIL".format(file_path))
            failed_files.append((file_path, error))
        else:
            print("- {}: PASS".format(file_path))

    if failed_files:
        print("\nFailed npk.yml files with reasons:")
        for file_path, error in failed_files:
            print("- {}: {}".format(file_path, error))

        # Exit with a non-zero status code if there were failures
        sys.exit(1)

    # Exit with a zero status code if all files are valid
    sys.exit(0)

if __name__ == "__main__":
    main()
