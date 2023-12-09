import os.path
from copy import deepcopy
from xml.etree import ElementTree as ET
import locale


def merge_ts_files(language_folder):
    proj_name = os.path.abspath(os.path.join(language_folder, os.path.pardir, os.path.pardir)).split('\\')[-1]
    merged_ts_file = f"{proj_name}_{language_folder}.ts"
    try:
        country_code = locale.normalize(language_folder).split('_')[1].split('.')[0]
    except:
        return

    root = ET.Element("TS")
    root.set("version", "2.1")
    root.set("language", f'{language_folder}_{country_code}')

    lang_path = os.path.abspath(language_folder)
    for root_folder, _, files in os.walk(lang_path):
        for file in files:
            if file.endswith(".ts"):
                ts_file_path = os.path.join(root_folder, file)

                # Parse each .ts file
                tree = ET.parse(ts_file_path)

                # Copy the root element and append the copy to the new root
                for element in tree.getroot():
                    root.append(deepcopy(element))

    # Create a new tree with the merged content
    merged_tree = ET.ElementTree(root)

    # Write the merged content to the new .ts file
    merged_tree.write(merged_ts_file, encoding="utf-8", xml_declaration=True)

    # print(f"Merged .ts files for {language_folder} into {merged_ts_file}")


translations_folder = os.path.abspath(os.curdir)
# Iterate over language folders
for language_folder in os.listdir(translations_folder):
    if os.path.isdir(os.path.join(translations_folder, language_folder)):
        merge_ts_files(language_folder)
