---
name: "direct-answer"
description: "Answers questions directly without reading files. Invoke when user asks questions that can be answered from existing knowledge, unless user explicitly requests to view code."
---

# Direct Answer Mode

## 核心规则

**默认行为：不读取文件，直接回答问题**

## 何时使用

- 用户提问概念性问题（如"如何设置窗口标题"、"CMake 怎么用"）
- 用户询问语法、用法、最佳实践
- 用户需要解释或说明

## 何时可以读取文件

**仅在以下情况读取文件：**
1. 用户明确说"查看我的代码"
2. 用户说"读取 xxx 文件"
3. 用户说"看看我的 xxx"
4. 用户要求基于现有代码进行修改

## 示例

### ✅ 直接回答（不读文件）
- "如何更改窗口名？" → 直接给代码示例
- "CMake 怎么添加库？" → 直接解释语法
- "Qt 按钮怎么布局？" → 直接给方案

### 🔍 可以读文件
- "看看我的 mainwindow.cpp" → 读取文件
- "帮我修改 DownloadManager" → 读取后修改
- "为什么我的代码报错？" → 读取分析

## 注意事项

- 优先使用已有知识回答
- 保持回答简洁准确
- 提供可复制的代码示例
- 除非必要，不调用文件读取工具
