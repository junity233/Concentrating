# Concentration Lua API文档

## 一.Concen

Concen包含了Concentraing的核心功能，包括禁用/启用鼠标/键盘，进程处理，打开/关闭浏览器等.Concen直接包含的函数有以下几个：

### 1.Concen.wait(s)

让程序暂停s毫秒

### 2.Concen.wait_until(hour [,minute [,second]])

暂停，直到时间大于hour:minute:second。当minute和second未指定时，其默认为0。

### 3.Concen.log(...)

向程序的 “日志” 界面输出一条消息，相邻两个参数使用空格隔开。该函数仅能处理以下类型的参数：

1. number
2. string
3. table (显示为[TABLE])
4. nil (显示为nil)
5. boolean (显示为True或False)

### 3.Concen.exec(command)

将command发送到shell执行。此函数会在底层调用system函数来实现

### 4.Concen.shutdown()

使电脑关机

### 5.Concen.msgbox(msg [,title [,type]])

显示一个对话框，其标题为title，内容为msg，样式为type。其中，type可以通过Msgbox中的值来指定，例如：

```lua
Concen.msgbox("Hello world","Test",Msgbox.Ok)
```

由于底层是通过MessageBox函数来实现，故type的值也和WinApi中的意义相同:Msgbox.Ok就相当于MB_OK。同样的，任何的MB_XXX都可以使用Msgbox.Xxx(首字母大小)来表示。

缺省时，title为"Concentrating",type为0。

此对话框是模态的。

### 6.Concen.message(msg [,title])

使用系统托盘发送一条信息，标题为title，内容为msg。

缺省时，title为"Concentrating"

### 7.Concen.run_script(script)

执行一个用户编写的脚本，若script为整型，则执行对应编号的脚本；若script为字符串，则执行对应名称的第一个脚本；其他情况下将得到一个错误。

### 8.Concen.process_event()

处理事件循环。（貌似现在没什么用）

### 9.Concen.make_time(hour [,minute [,second]])

创建一个表示时间的表。

缺省时，minuts、second的值为0

### 10.Concen.beep(freq,dur)

使用蜂鸣器播放声音，频率为freq，持续时间为dur

### 11.Concen.import()

将Concen库中的所有内容导出到全局中。导出后，调用函数可以不用加"Concen"的前缀。如可以这样调用log函数：

```lua
log("Hello world")
```

## 二. Concen.mouse

Concen.mouse中包含了几个与鼠标有关的函数

### 1.Concen.mouse.lock()

锁定鼠标

### 2.Concen.mouse.unlock()

解锁鼠标

### 3.Concen.mouse.pos()

获取鼠标的位置。该函数返回值有两个，分别为X、Y坐标。

### 4.Concen.mouse.move(x,y)

将鼠标移动到(x,y)处

## 三.Concen.keyboard

包含与键盘有关的函数

### 1.Concen.keyboard.lock()

锁定键盘，只有指定的键可以被按下。

### 2.Concen.keyboard.unlock()

解锁键盘

### 3.Concen.keyboard.get_key_status(key)

获取键key在键盘锁定时是否可以被按下。

key的值为键对应的虚拟键码，可以通过Vkcode来获取。

例如，空格键的虚拟键码可以用 "Vkcode.space" 来表示。

### 4.Concen.keyboard.set_key_status(key,enable)

设置键key是否能被按下

### 5.Concen.keyboard.enable_keys(...)

将参数指定的所有键设为可以按下。

### 6.Concen.keyboard.enable_keys(keys)

该函数为Concen.keyboard.enable_keys(...)的重载形式。

将keys表中值对应的所有键设为可以按下。

### 7.Concen.keyboard.disable_keys(...)

将参数指定的所有键设为不能按下。

### 8.Concen.keyboard.disable_keys(keys)

该函数为Concen.keyboard.disable_keys(...)的重载形式。

将keys表中值对应的所有键设为不能按下。

## 三.Concen.browser

包含与内置浏览器有关的操作。

### 1.Concen.browser.open()

打开浏览器

### 2.Concen.browser.close()

关闭浏览器。此方法执行后，浏览器并不会真正地关闭，而是会在后台继续运行（仅隐藏窗口）

### 3.Concen.browser.load(url)

加载url指定的网页

### 4.Concen.browser.set_allowed_hosts(hosts)

允许hosts表中值对应的所有hosts的网站被访问。例如，以下代码允许acwing.com,www.acwing.com,123.acwing.com被访问：

```lua
Concen.browser.set_allowed_hosts({"*.acwing.com","acwing.com"})
```

hosts支持通配符。

### 5.Concen.browser.set_default_url(url)

设置浏览器的默认页面。

### 6.Concen.browser.url()

返回当前浏览器当前页面的网址。

## 四.Concen.setting

包含与设置相关的操作

### 1.Concen.setting.read(key)

以key为键，读取一个设置。

### 2.Concen.setting.write(key,value)

以key为键，写入value到设置中。

### 3.Concen.setting.remove(key)

移除键为key的设置项

### 4.Concen.setting.exist(key)

若存在键为key的项，返回true；否则返回false

## 五.Concen.process

包含与进程相关的操作

### 1.Concen.process.create(exec [,cmd])

创建一个进程，并返回其PID。

其中，exec为程序的路径，cmd为要传给程序的命令行参数。

### 2.Concen.process.find(name)

查找以name为进程名的所有进程，并返回一个表，包含这些进程的PID

### 3.Concen.process.kill(pid)

结束pid对应的进程，成功返回true，失败返回false。

## 六. Concen.speaker

包含与文本转语音（TTS）有关的操作。

### 1.Concen.speaker.say(text)

让电脑朗读text

### 2.Concen.speaker.set_volume(vol)与Concen.speaker.volume()

设置/查询当前文本转语音的音量，音量是一个整数，范围从-100到100

### 3.Concen.speaker.set_rate(rate)与Concen.speaker.rate()

设置/查询当前文本转语音的速度，速度是一个整数，范围从-100到100

### 4.Concen.speaker.pause()

暂停当前的朗读，可以在之后使用Concen.speaker.resume()来继续。

### 5.Concen.speaker.stop()

终止朗读

### 6.Concen.speaker.resume()

继续由Concen.speaker.pause()暂停的朗读。

### 7.Concen.speaker.state()

获取TTS系统的状态。返回值是一个0-3的整数，意义如下：

0. 合成器准备开始新的文本。这也是一个文本结束后的状态。
1. 现在正在朗读文本
2. 朗读被暂停
3. 内部错误
