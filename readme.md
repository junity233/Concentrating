# Concentrating

[TOC]

## 简介

Concentrating是一个基于脚本的自律软件，可以使用lua脚本来实现不同的功能，配合自启动和任务表

可以做到把一天安排地明明白白。

## 使用说明

### 脚本

脚本系统是本软件的核心功能，可以通过运行脚本来控制鼠标、键盘或者发送消息，弹出对话框等。

每一个脚本都会有一个名称和一个编号。其中，用户通过名称来区分脚本，而软件内部则是通过其编号来区分，因此在设置界面中选择脚本时要特别注意。

脚本的名称可以重复，但不建议这样做，因为目前脚本存储在"scripts.json"中，但后来可能会将其直接存储到文件夹中。

运行脚本有两种方法，一种是在脚本界面点击“运行脚本”，另一种是在任务表中添加一个关于这个脚本的任务。有关任务表的内容会在之后的部分讲解。同一时间可以运行多个不同的脚本。

脚本在运行时可能会输出一些信息，这些信息可以在日志界面中看到。同样的，若脚本在运行时发生错误（例如语法错误或者运行时错误），错误信息也将在日志界面中显示。

Lua API参见 [Lua API](Lua API.md)

### 设置

软件中的所有设置都保存在一个类似Json的树中。脚本和用户都可以对此进行读写，用户可以在设置界面的"脚本"页面中编辑。

对于脚本而言，只能通过Concentrating.setting.read/write方法来读取设置，但要注意的是，参数key实际上是一个路径，如有类似以下Json的设置：

```json
{
    "a":{
        "b":{
            "c":{
                "d":true
            }
        }
    }
}
```

以下脚本将返回true：

```lua
Concen.setting.read("a.b.c.d")
```

设置项“system”内保存了软件的相关设置。

#### 密码

Concentrating可以配置密码，方法是在设置->一般->访问 中开启 “需要密码”，并在下方输入密码。

开启密码后，关闭及显示Concentrating主窗口都将需要密码。

### 浏览器

Concentrating内置了一个基于chromium的浏览器，但由于Qt官方在编译时没有开启某些编码库的编译选项，故可能无法播放某些视频。

内置浏览器有一个白名单，只有在白名单内的网站才能被访问。白名单中保存了域名的Host部分，这样可以方便地将整个网站放行。白名单支持通配符。

e.g：*.acwing.com 将允许a.acwing.com,b.acwing.com/12345，但不允许acwing.com（通配符）

### 任务表

任务表系统可以配置定时任务，配置的方法是在设置界面的任务表界面中添加一个任务并启用。

任务的时间项类似Linux下的crontab，由六个项组成，从左到右分别表示：

1. 秒
2. 分
3. 时
4. 天（相对于月）
5. 月
6. 星期几

项与项之间由空格隔开，每个项由一个字符串组成。

每个项可以用以下方式指定其值：

1. 数字。例如，"0 0 7 1 1 1"可以表示在一月一日星期一（如果是的话）执行。
2. 星号，表示所有值。例如，"* * * * * *"会每隔一秒执行一次。
3. 范围，格式为“%-%"，其中%代表一个数字。例如，"0 0 7-13 * * *"将在每天的7点到13点，每隔一个小时执行一次
4. 逗号，可以连接多个值。例如，“1,2,3,4,5 * * * * *"将在每分钟的前5秒每秒执行一次
5. 间隔。通过在范围表达式的末尾添加一个"/%"，可以实现指定范围的间隔。例如，"1-13/2"可以表示1,3,5,7,9,11,13几个数

[Lua API.md]: