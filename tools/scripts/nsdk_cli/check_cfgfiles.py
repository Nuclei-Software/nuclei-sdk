#!/usr/bin/env python

import os
import sys
import yaml
import json
import yamale
import re
import argparse

def find_files_regex(directory, pattern):
    """
    Recursively search the specified directory and its subdirectories for files with the given name.

    :param directory: The path to the directory to search in.
    :param pattern: The file name regex pattern.
    :return: A list of paths to all found files.
    """
    found_files = []
    for root, dirs, files in os.walk(directory):
        for filename in files:
            if re.match(pattern, filename):
                found_files.append(os.path.join(root, filename))
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

def validate_json(file_path):
    """
    Validate a JSON file to check if it is a valid JSON document.

    :param file_path: The path to the JSON file.
    :return: A tuple containing a boolean (True if the file is valid, False otherwise) and an error message (if the file is invalid).
    """
    try:
        with open(file_path, 'r') as file:
            json.load(file)
            return True, None
    except json.JSONDecodeError as exc:
        return False, exc

def validate_yaml_schema(file_path):
    """
    Validate a yaml schema file to check if it is a valid schema document.

    :param file_path: The path to the yaml schema file.
    :return: A tuple containing a boolean (True if the file is valid, False otherwise) and an error message (if the file is invalid).
    """
    try:
        yamale.make_schema(file_path)
        return True, None
    except Exception as exc:
        return False, exc

def validate_cfgfile(file_path):
    """
    Validate a config file to check if it is a valid configuration file.

    :param file_path: The path to the configuration file.
    :return: A tuple containing a boolean (True if the file is valid, False otherwise) and an error message (if the file is invalid).
    """
    file_name = os.path.basename(file_path)
    fname = file_name.split(".")[0]
    suffix_keys = file_name.split(".")[1:]

    if "json" in suffix_keys:
        return validate_json(file_path)
    if "schema" in fname:
        return validate_yaml_schema(file_path)
    return validate_yaml(file_path)

def main():
    parser = argparse.ArgumentParser(description="Validate json and yaml configuration files.")
    parser.add_argument("-d", "--directory", required=True, help="The directory to search for json and yaml files.")
    args = parser.parse_args()

    # Check if the directory exists
    if not os.path.isdir(args.directory):
        print("The directory {} does not exist!".format(args.directory))
        sys.exit(1)

    pattern = r'^.*\.(json|json\..*|yaml|yml)$'
    config_files = find_files_regex(args.directory, pattern)
    failed_files = []

    for file_path in config_files:
        is_valid, error = validate_cfgfile(file_path)
        if not is_valid:
            print("- {}: FAIL".format(file_path))
            failed_files.append((file_path, error))
        else:
            print("- {}: PASS".format(file_path))

    if failed_files:
        print("\nFailed config files with reasons:")
        for file_path, error in failed_files:
            print("- {}: {}".format(file_path, error))

        # Exit with a non-zero status code if there were failures
        sys.exit(1)

    # Exit with a zero status code if all files are valid
    sys.exit(0)

if __name__ == "__main__":
    main()
