import os
import re
import yaml

from PyQt5.QtCore import QObject, pyqtSlot

yaml_file = 'config.yaml'
output_yaml = 'custom.yaml'
temp_yaml = 'temp.yaml'
map_yaml = 'map.yaml'

actual_language_map = None

# 定义宏名称与原来语言ID的映射字典
language_id_map = {
    'ARABIC': 'ar_ril', 
    'BENGALI': 'bn_rbn', 
    'CHINESE': 'zh_rcn', 
    'CZECK': 'cs_rcs', 
    'DUTCH': 'nl_rnl', 
    'ENGLISH': 'en_rus', 
    'FRENCH': 'fr_rfr', 
    'GERMAN': 'de_rde', 
    'HINDI': 'hi_rin', 
    'INDIA': 'hi_rin', 
    'INDONESUAN': 'in_rid', 
    'ITALIAN': 'it_rit', 
    'JAPANESE': 'ja_rjp', 
    'KOREAN': 'ko_rkr', 
    'POLISH': 'pl_rpl', 
    'PORTUGUESE': 'pt_rpt', 
    'PUNJABI': 'pa_rpa', 
    'ROMANIAN': 'ro_rro', 
    'RUSSIAN': 'ru_rru', 
    'SPANISH': 'es_res', 
    'THAI': 'th_rth', 
    'TRADITIONAL_CHINESE': 'zh_rtw', 
    'TURKISH': 'tr_rtr', 
    'URDU': 'ur_rur', 
    'VIETNAMESE': 'vi_rvn', 
    'MALAYSIA': 'ms_rmy', 
    'GREEK': 'el', 
    'FARSI': 'fa_rir'
}

def process_yaml(yaml_data, folder_path):
    for entry in yaml_data:
        text_parts = entry['text'].split()
        region_code = text_parts[0].lower()  # 将 region_code 转为小写

        file_name = f"ute_multi_language_{region_code}.c"

        if not os.path.exists(os.path.join(folder_path, file_name)):
            entry['checked'] = False


def process_yaml_alt(yaml_data, language_code, number):
    for entry in yaml_data:
        text_parts = entry['text'].split()
        region_code = text_parts[0].lower()

        if region_code == language_code:
            entry['checked'] = int(number) == 1


def process_yaml_uncheck(yaml_data):
    for entry in yaml_data:
        entry['checked'] = False


class Autocfg(QObject):
    def __init__(self, parent=None):
        super().__init__(parent)

    @pyqtSlot(str)
    def by_filename(self, folder_path):
        with open(yaml_file, 'r', encoding='utf-8') as file:
            yaml_data = yaml.safe_load(file)

        process_yaml(yaml_data, folder_path)

        with open(output_yaml, 'w', encoding='utf-8') as file:
            yaml.dump(yaml_data, file, default_flow_style=False)

    @pyqtSlot(str)
    def by_header(self, header_file):
        with open(header_file, 'r', encoding='utf-8') as file:
            config_data = file.readlines()

        with open(yaml_file, 'r', encoding='utf-8') as file:
            yaml_data = yaml.safe_load(file)

        # 检查配置文件是否存在，如果不存在则生成
        if not os.path.exists(map_yaml):
            print(f"Creating {map_yaml}...")  # 添加调试信息
            with open(map_yaml, 'w', encoding='utf-8') as file:
                print(f"Writing to {map_yaml}: {language_id_map}")  # 添加调试信息
                yaml.dump(language_id_map, file, default_flow_style=False)
            print(f"{map_yaml} created and written.")  # 添加调试信息

        # 读取配置文件
        with open(map_yaml, 'r', encoding='utf-8') as file:
            language_id_mapping = yaml.safe_load(file)

        if language_id_mapping == None:
            language_id_mapping = language_id_map

        global actual_language_map
        actual_language_map = language_id_mapping

        pattern = re.compile(r'#define SCREEN_TITLE_MULTIPLE_(\w+)_LANGUAGE_SUPPORT\s+(\d+)')

        process_yaml_uncheck(yaml_data)
        for line in config_data:
            match = pattern.match(line)
            if match:
                language_code, number = match.group(1).upper(), match.group(2)  # 使用大写匹配字典中的键
                original_language_id = language_id_mapping.get(language_code)
                if original_language_id:
                    process_yaml_alt(yaml_data, original_language_id, number)

        with open(output_yaml, 'w', encoding='utf-8') as file:
            yaml.dump(yaml_data, file, default_flow_style=False)

    @pyqtSlot()
    def unselect_all(self):
        with open(yaml_file, 'r', encoding='utf-8') as file:
            yaml_data = yaml.safe_load(file)

        process_yaml_uncheck(yaml_data)

        with open(temp_yaml, 'w', encoding='utf-8') as file:
            yaml.dump(yaml_data, file, default_flow_style=False)

    @pyqtSlot(str, result=str)
    def query_language_id(self, language_name):
        """
        根据语言名称查询对应的语言ID
        
        Args:
            language_name (str): 语言名称（如 'CHINESE', 'ENGLISH' 等）
            
        Returns:
            str: 对应的语言ID，如果未找到则返回空字符串
        """
        # 使用实际加载的配置，如果不存在则使用默认配置
        global actual_language_map
        current_map = actual_language_map if actual_language_map is not None else language_id_map
        language_id = current_map.get(language_name.upper(), "")
        return language_id

    @pyqtSlot(result=str)
    def get_all_language_map(self):
        """
        获取完整的语言映射字典
        
        Returns:
            str: YAML格式的语言映射字典字符串
        """
        # 使用实际加载的配置，如果不存在则使用默认配置
        global actual_language_map
        current_map = actual_language_map if actual_language_map is not None else language_id_map
        return yaml.dump(current_map, default_flow_style=False)

    @pyqtSlot(str, result=str)
    def get_language_name_by_id(self, language_id):
        """
        根据语言ID查询对应的语言名称
        
        Args:
            language_id (str): 语言ID（如 'zh_rcn', 'en_rus' 等）
            
        Returns:
            str: 对应的语言名称，如果未找到则返回空字符串
        """
        # 使用实际加载的配置，如果不存在则使用默认配置
        global actual_language_map
        current_map = actual_language_map if actual_language_map is not None else language_id_map
        # 创建反向映射字典（从语言ID到语言名称）
        id_to_language_map = {v: k for k, v in current_map.items()}
        language_name = id_to_language_map.get(language_id, "")
        return language_name
    