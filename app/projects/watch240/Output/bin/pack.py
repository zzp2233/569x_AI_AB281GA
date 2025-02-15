import os
import shutil
import subprocess
import glob
import re
from datetime import datetime
import xml.etree.ElementTree as ET

# 定义固定内容（sys.cfg 的内容）
SYS_CFG_CONTENT = b"DUF-UIAB"

sdk_name = 'AB5691_SDK'
config_name = 'project_config'
project_version = ''

def update_watch_setting(file_path):
    # 定义要修改的键值对
    replacements = {
        "bt_addr_mode": "单次递增",
        "bt_addr": "78:02:B7:00:00:01",
        "bt_addr_start": "78:02:B7:00:00:01",
        "bt_addr_step": "00:00:00:00:00:01",
        "bt_addr_end": "78:02:B7:FF:FF:FF"
    }

    # 解析 XML 文件
    tree = ET.parse(file_path)
    root = tree.getroot()

    # 遍历所有元素
    for add_element in root.findall(".//add"):
        key = add_element.get("key")
        if key in replacements:
            # 更新 value 属性
            add_element.set("value", replacements[key])

    # 保存修改后的 XML 文件
    tree.write(file_path, encoding="utf-8", xml_declaration=True)

# 在生成 DFU_UI_CODE.bin 后，再次调用 Downloader.exe 生成 code.prd
def execute_downloader_prd(downloader_path, app_dcf_path, watch_setting_path, key_file, code_prd_path):
    cmd = [
        downloader_path,
        "-b",
        "-o",
        code_prd_path,
        "-i",
        app_dcf_path,
        "-s",
        watch_setting_path,
        "-k",
        key_file
    ]
    try:
        subprocess.check_call(cmd, cwd=os.path.dirname(downloader_path))
    except subprocess.CalledProcessError:
        print("错误: 执行 Downloader.exe 生成 code.prd 失败。")
        return 1
    return 0

def find_project_config(script_dir, sdk_name, config_name):
    # 从脚本目录开始向上查找 AB5691_SDK 目录
    current_dir = script_dir
    while current_dir != os.path.dirname(current_dir):
        sdk_path = os.path.join(current_dir, sdk_name)
        if os.path.exists(sdk_path):
            config_path = os.path.join(sdk_path, 'ute', config_name)
            if os.path.exists(config_path):
                return config_path
        current_dir = os.path.dirname(current_dir)
    return None

def get_ute_project(filename):
    with open(filename, 'r', errors='ignore') as file:
        content = file.read()

    match = re.search(r'#define\s+PROJECT_(\w+)_SUPPORT\s+1', content)
    print("name:match", match)
    if match:
        ute_name = match.group(1)
        # print("ute_name:", ute_name)
    else:
        ute_name = "default"
        # print("ute_name is default")
    return ute_name

def get_ute_version(filename):
    with open(filename, 'r', errors='ignore') as file:
        content = file.read()

    match = re.search(r'#define\s+UTE_SW_VERSION\s+"(\w+)"', content)
    print("version:match", match)
    if match:
        ute_version = match.group(1)
        print("version:", ute_version)
    else:
        print("version is null")
        ute_version = ''
    return ute_version

def get_ute_custom_patch(filename):
    try:
        with open(filename, 'r', encoding='utf-8') as file:
            content = file.read()
    except Exception as e:
        print(f"Error: An error occurred while reading the file {filename}: {e}")
        return ""
    
    pattern = r'#define\s+UTE_UI_CONFIG_PATCH\s+"([^"]+)"'
    try:
        match = re.search(pattern, content)
    except re.error as e:
        print(f"Error: A regex error occurred: {e}")
        return ""
    
    if match:
        print("UTE_UI_CONFIG_PATCH macro found:", match.group(1))
        return match.group(1)
    else:
        print("UTE_UI_CONFIG_PATCH macro not found.")
        return ""

def generate_code_fot(downloader_path, app_dcf_path, watch_setting_path, key_file, code_fot_path):
    """生成 code.fot 文件"""
    # 删除旧的 code.fot 文件（如果存在）
    if os.path.exists(code_fot_path):
        try:
            os.remove(code_fot_path)
        except Exception as e:
            print(f"错误: 删除 code.fot 时出错 - {e}")
            return 1

    # 构建调用 Downloader.exe 的参数
    cmd = [
        downloader_path,
        "-b",
        "-o",
        code_fot_path,
        "-i",
        app_dcf_path,
        "-s",
        watch_setting_path,
        "-k",
        key_file  # 传递单个 .key 文件
    ]

    # 调用 Downloader.exe
    try:
        subprocess.check_call(cmd, cwd=os.path.dirname(downloader_path))
    except subprocess.CalledProcessError:
        print("错误: 执行 Downloader.exe 生成 code.fot 失败。")
        return 1

    return 0

def get_current_time_formatted():
    # 获取当前时间
    now = datetime.now()
    # 格式化时间为 yyyymmdd 格式
    formatted_time = now.strftime("%Y%m%d")
    return formatted_time

def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    output_dir = os.path.join(script_dir, "Output")  # 新建的输出目录
    downloader_dir = os.path.join(script_dir, "Downloader")
    downloader_path = os.path.join(downloader_dir, "Downloader.exe")
    app_dcf_path = os.path.join(script_dir, "app.dcf")
    watch_setting_path = os.path.join(script_dir, "Settings", "watch.setting")
    settings_target = os.path.join(downloader_dir, "Settings")
    key_dir = os.path.join(downloader_dir, "key")  # 新增的 key 目录路径

    project_config_path = find_project_config(script_dir, sdk_name, config_name)
    project_config = os.path.join(project_config_path, 'ute_project_config.h')
    if not os.path.exists(project_config):
        print("项目配置文件未找到")
        return 1
    else:
        project_name = get_ute_project(project_config)
        custom_patch = os.path.join(project_config_path, 'ute_project_config_' + project_name.lower() + '.h')
        print("project_name:", project_name)
        print("custom_patch:", custom_patch)
        project_version = get_ute_version(custom_patch)
        if project_version == '':
            print("版本号未找到")
            project_version = ''

    code_fot_name = project_version + '_OTA_CODE_' + get_current_time_formatted() + '.fot'
    code_prd_name = project_version + '_MP_' + get_current_time_formatted() + '.prd'
    firmware_name = project_version + '_OTA_UI_CODE_' + get_current_time_formatted() + '.bin'
    code_fot_path = os.path.join(output_dir, code_fot_name)
    code_prd_path = os.path.join(output_dir, code_prd_name)  # code.prd 保存到 Output 目录
    dfu_ui_code_path = os.path.join(output_dir, firmware_name)  # DFU_UI_CODE.bin 保存到 Output 目录

    # 创建 Output 目录（如果不存在）
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    # 检查关键文件和目录是否存在
    # 确保 Downloader.exe 存在于 Downloader 目录下
    if not os.path.exists(downloader_path):
        print("错误: Downloader.exe 不存在。请检查 Downloader 目录下是否有 Downloader.exe 文件。")
        return 1

    if not os.path.exists(app_dcf_path):
        print("错误: app.dcf 文件不存在。")
        return 1

    if not os.path.exists(watch_setting_path):
        print("错误: watch.setting 文件不存在。")
        return 1

    if not os.path.exists(os.path.join(script_dir, "Settings")):
        print("错误: Settings 目录不存在。")
        return 1

    # 检查 key 目录是否存在
    if not os.path.exists(key_dir):
        print("错误: key 目录不存在。")
        return 1

    # 获取 key 目录下的所有 .key 文件
    key_files = glob.glob(os.path.join(key_dir, "*.key"))
    
    # 检查是否存在多个 .key 文件
    if not key_files:
        print("错误: key 目录下没有 .key 文件。")
        return 1
    elif len(key_files) > 1:
        print("错误: key 目录下有多个 .key 文件。请确保只存在一个。")
        return 1

    # 删除旧的 code.fot 文件（如果存在）
    if os.path.exists(code_fot_path):
        try:
            os.remove(code_fot_path)
        except Exception as e:
            print(f"错误: 删除 code.fot 时出错 - {e}")
            return 1
        
    # 删除旧的 code.prd 文件（如果存在）
    if os.path.exists(code_prd_path):
        try:
            os.remove(code_prd_path)
        except Exception as e:
            print(f"错误: 删除 code.prd 时出错 - {e}")
            return 1
        
    # 复制 app.dcf 到 Downloader 目录
    if not os.path.exists(os.path.join(downloader_dir, "app.dcf")):
        try:
            shutil.copy2(app_dcf_path, downloader_dir)
        except FileNotFoundError as e:
            print(f"错误: 复制 app.dcf 文件失败 - {e}")
            return 1

    # 删除旧的 Settings 目录（如果存在）
    if os.path.exists(settings_target):
        try:
            shutil.rmtree(settings_target)
        except OSError as e:
            print(f"错误: 删除旧的 Settings 目录时出错 - {e}")
            return 1

    # 复制 Settings 目录
    try:
        shutil.copytree(os.path.join(script_dir, "Settings"), settings_target)
    except FileExistsError:
        print("错误: Settings 目录复制失败（目标目录可能未完全删除）")
        return 1
    
    # 修改复制后的 watch.setting 文件内容
    modified_watch_setting_path = os.path.join(settings_target, "watch.setting")
    if os.path.exists(modified_watch_setting_path):
        update_watch_setting(modified_watch_setting_path)
    else:
        print("错误: 复制后的 watch.setting 文件不存在。")
        return 1

    # 调用生成 code.fot 的函数
    if generate_code_fot(downloader_path, app_dcf_path, watch_setting_path, key_files[0], code_fot_path) != 0:
        return 1

    # 移动生成的 code.fot 到 Output 目录
    code_fot_in_downloader = os.path.join(downloader_dir, code_fot_name)
    if os.path.exists(code_fot_in_downloader):
        try:
            shutil.move(code_fot_in_downloader, code_fot_path)
        except Exception as e:
            print(f"错误: 移动生成的 code.fot 文件时出错 - {e}")
            return 1

    print("成功生成:", code_fot_name)

    # 删除已存在的固件文件
    firmware_files = ["DFU_UI_CODE.bin", "DFU_CODE.bin", firmware_name]
    for file in firmware_files:
        if os.path.exists(os.path.join(output_dir, file)):
            try:
                os.remove(os.path.join(output_dir, file))
            except OSError as e:
                print(f"错误: 无法删除文件 {file} - {e}")

    # 检查 ui.bin 和 code.fot 是否存在
    ui_bin_path = os.path.join(script_dir, "ui.bin")
    if not os.path.exists(ui_bin_path):
        print("错误: ui.bin 文件不存在。")
        return 1
    if not os.path.exists(code_fot_path):
        print("错误: code.fot 文件不存在。")
        return 1

    # 创建 ui_size.cfg
    ui_size_cfg_path = os.path.join(output_dir, "ui_size.cfg")
    try:
        with open(ui_size_cfg_path, "w") as f:
            f.write(str(os.path.getsize(ui_bin_path)))
    except OSError as e:
        print(f"错误: 无法创建 ui_size.cfg 文件 - {e}")
        return 1

    # 合并文件生成 DFU_UI_CODE.bin
    try:
        with open(dfu_ui_code_path, "wb") as outfile:
            # 直接写入固定内容（取代 sys.cfg）
            outfile.write(SYS_CFG_CONTENT)
            
            # 写入 ui_size.cfg 文件
            with open(ui_size_cfg_path, "rb") as infile:
                shutil.copyfileobj(infile, outfile)
            
            # 写入 ui.bin 文件
            with open(ui_bin_path, "rb") as infile:
                shutil.copyfileobj(infile, outfile)
            
            # 写入 code.fot 文件
            with open(code_fot_path, "rb") as infile:
                shutil.copyfileobj(infile, outfile)
    except OSError as e:
        print(f"错误: 文件合并失败 - {e}")
        return 1
    
    print("成功生成:", firmware_name)

    # 清理临时文件
    os.remove(ui_size_cfg_path)

    # 成功生成 DFU_UI_CODE.bin 后，调用 Downloader.exe 生成 code.prd
    if execute_downloader_prd(downloader_path, app_dcf_path, watch_setting_path, key_files[0], code_prd_path) != 0:
        return 1

    print("成功生成:", code_prd_name)

    tmp_dcf_patch = os.path.join(downloader_dir,'app.dcf')
    if os.path.exists(tmp_dcf_patch):
        try:
            os.remove(tmp_dcf_patch)
        except Exception as e:
            print(f"错误: 删除 app.dcf 时出错 - {e}")

    return 0

if __name__ == "__main__":
    exit(main())