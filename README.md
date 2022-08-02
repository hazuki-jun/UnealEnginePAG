# PAGSample

Developed with Unreal Engine 5

集成PAG的UE5插件-提供从AE制作的动画视频直接导出的PAG拖入到UE5中使用


## PAG介绍

[PAG官网](https://pag.io)
| [QQ群：893379574](https://qm.qq.com/cgi-bin/qm/qr?k=Wa65DTnEKo2hnPsvY-1EgJOF8tvKQ-ZT&jump_from=webapi)

libpag 是 PAG (Portable Animated Graphics) 动画文件的渲染 SDK，目前已覆盖几乎所有的主流平台，包括：iOS, Android, macOS,
Windows, Linux, 以及 Web 端。

PAG 方案是一套完善的动画工作流。提供从 AE（Adobe After Effects）导出插件，到桌面预览工具 PAGViewer，再到各端的跨平台渲染 SDK。
目标是降低或消除动画研发相关的成本，打通设计师创作到素材交付上线的极简流程，不断输出运行时可编辑的高质量动画内容。

PAG 方案目前已经接入了腾讯系 40 余款应用，包括微信，手机QQ，王者荣耀，腾讯视频，QQ音乐等头部产品，稳定性经过了海量用户的持续验证，可以广泛应用于UI动画、贴纸
动画、视频编辑、模板设计等场景。典型应用场景可以参考[官网案例](https://pag.io/case.html)。

详细介绍可以参考相关报道：
 
 - [王者QQ微信都在用的动画神器要开源了：把交付时间缩短90%](https://mp.weixin.qq.com/s/a8-yOp8h5LiFGKSdLE_toA)
 - [腾讯推出移动端动画组件PAG，释放设计生产力](https://mp.weixin.qq.com/s/STxOMV2lqGdGu-9mBkAz_A)

## PAG 优势

- **高效的文件格式**

采用可扩展的二进制文件格式，可单文件集成图片音频等资源，实现快速交付。导出相同的 AE 动画内容，在文件解码速度和压缩率上均大幅领先于同类型方案。

- **全 AE 特性支持**

在纯矢量导出方式上支持更多 AE 特性的同时，还引入了BMP预合成结合矢量的混合导出能力，实现支持所有 AE 特性的同时又能保持动画运行时的可编辑性。

- **性能监测可视化**

通过导出插件内置的自动优化策略，以及预览工具集成的性能监测面板，能够直观地看到每个素材的性能状态，以帮助设计师制作效果和性能俱佳的动画特效。

- **运行时可编辑**

运行时，可在保留动画效果前提下，动态修改替换文本或替换占位图内容，甚至对任意子图层进行增删改及移动，轻松实现照片和视频模板等素材的批量化生产。


