# Git Commit 重写方案

## 当前问题
- 大量 commit message 含"第X天/昨天/前天"等日期描述
- z_* 目录使用旧名称(z_new_o/z_new_w/z_osabin)，描述也错误（"旧版工具"）
- qiu_* 用"第X天"而非功能增量描述

## 方案：git rebase -i 重写

以下命令会打开交互式编辑器，将每个 `pick` 改为 `reword`，输入新 message。

```bash
cd e:\30dayMakeOs\try_os
git rebase -i --root
```

---

## 新旧 commit message 映射表

按提交顺序（旧→新）：

### 第一组：初始化（原含"第1~4天"）

| Hash | 旧 Message | 新 Message |
|------|-----------|-----------|
| 5e624b6 | 初始化项目：工具链(z_tools) + 第1~4天源码 + 许可证 | 导入工具链(z_tools) + OS/Win项目模板(z_os_template/z_win_template) + OSASK参考(z_osask_ref) + 许可证 |
| a89bfd1 | 初始化项目：导入第1~4天源码 + 工具链(z_tools) + 许可证(GPL/LGPL) | → **squash 到上方**（amend） |

### 第二组：配置

| Hash | 旧 Message | 新 Message |
|------|-----------|-----------|
| 934f66d | 添加Copilot指令配置(.github/copilot-instructions.md) | 添加AI助手行为约束配置(copilot-instructions.md) |
| acbc165 | 添加Copilot指令配置(.github/copilot-instructions.md) | → **squash 到上方**（amend） |

### 第三组：清理

| Hash | 旧 Message | 新 Message |
|------|-----------|-----------|
| a8626af | 清理过期的prompt残留文件 | 清理过期prompt残留文件 |
| 1378749 | 清理过期的prompt残留文件 | → **squash 到上方**（amend） |

### 第四组：文档

| Hash | 旧 Message | 新 Message |
|------|-----------|-----------|
| 70c5a2f | 添加README.md项目说明 + docs/makefile-notes.md构建笔记 | 添加README.md项目说明 + 构建笔记(makefile-notes.md) |
| 1cba2c7 | 添加README.md项目说明 + 构建笔记(docs/makefile-notes.md) | → **squash 到上方**（amend） |

### 第五组：qiu_04/qiu_03 修复

| Hash | 旧 Message | 新 Message |
|------|-----------|-----------|
| 7647efe | 修复第4天三个edition的Makefile/make.bat路径 + 清理edition_3构建中间产物 | 修复qiu_04各edition Makefile路径(../../z_tools/) + 清理构建中间产物 |
| 19b6351 | 修复第3、4天Makefile/make.bat工具链路径 + 清理构建中间产物 | 修复qiu_03/qiu_04 Makefile工具链路径 + 清理构建产物 |

### 第六组：qiu_05 系列

| Hash | 旧 Message | 新 Message |
|------|-----------|-----------|
| fb7470a | 创建第5天目录：IPL引导 + asmhead启动头 + bootPack/naskfunc内核框架 | qiu_05: 从qiu_04复制→IPL引导+asmhead启动头+C内核框架（仅目录，等指示） |
| 62a6b9f | 第5天：IPL引导扇区 + asmhead启动头 + bootPack/naskfunc内核框架 | qiu_05: +BOOT_INFO结构体解引用 + 完整I/O函数族(io_in/out/cli/sti/eflags) + putfont8字体渲染 |
| b55f957 | 完善第5天：BOOTINFO结构体读取显示信息 + putfont8字符显示 + 桌面任务栏布局 | → **squash 到上方**（微调） |
| 6fd54f6 | 第5天：引入BOOTINFO结构体 + putfont8字符显示 + 桌面布局 | → **squash 到上方**（原始版本） |
| b7e9c7b | 完善第5天：统一中文注释风格 | qiu_05: 统一中文注释风格（函数头/参数/返回值） |
| 457ae5c | 第5天：完善代码注释 | → **squash 到上方**（amend） |

---

## 快速操作步骤

```bash
cd e:\30dayMakeOs\try_os

# 1. 备份
git tag backup-before-rewrite
git branch backup-before-rewrite

# 2. 交互式 rebase
git rebase -i --root

# 3. 在编辑器中：
#    - 把 amend 类 commit 的 `pick` 改为 `squash`（合并到上一个）
#    - 把需要改 message 的 `pick` 改为 `reword`
#    - 参考上方映射表修改每个 message

# 4. 推送（注意：这是强制推送！）
git push --force-with-lease origin main

# 5. 更新 tag（如果有的话也要更新）
git tag -f day04-final <新的commit-hash>
git tag -f day05-final <新的commit-hash>
git push --force origin day04-final day05-final
```

---

## ⚠️ 警告
- `git rebase -i --root` 会重写**全部历史**
- 如果有其他人 clone 了这个仓库，他们会遇到冲突
- 建议在没人协作时操作
- 完成后所有 tag 需要重新打
