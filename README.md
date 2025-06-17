[EN]
 This library solves the problem of having to set or determine drawing and touch detection separately on devices with M5Stack touch panels.  
Like HTML, it is possible to depend on parent elements for coordinates and colors, and since it consolidates the “multiple patterns of syntax that perform the same action” that are problematic in HTML, it enables more flexible and stable drawing.
　Additionally, this library does not redraw after initial rendering unless the update syntax is called, thereby reducing resource usage per frame.  
Caution: Touch detection requires monochrome rendering on a sprite of the same size as the device's screen, so some memory is required.  
This library is based on M5Unified.

[JP]
　M5Stackのタッチパネルが存在するデバイスにおいて、描画とタッチ判定をそれぞれ別々で設定または判定しなければならない問題を解決するライブラリです。
　HTMLのように親要素に座標や色などを依存させることが可能であり、かつHTMLで問題となる「同じ動作をする複数パターンの構文」を統括したため、より自由度と安定性の高い描画を行うことができます。
　また、このライブラリは描画において一度描画した後は更新するための構文が呼び出さない限り再描画を行わないため、フレーム毎のリソースを軽減できます。
　Caution：タッチ判定には裏でデバイスの画面サイズと同じサイズのスプライトにモノクロ描画を行うため、多少のメモリが必要になります。
　このライブラリはM5Unifiedをベースに制作しています。

[CN]
　该库解决了在配备 M5Stack 触摸屏的设备上必须分别设置或判断绘图和触摸检测的问题。
它可以像 HTML 一样依赖父元素的坐标和颜色等，并且整合了 HTML 中存在问题的“执行相同操作的多种模式语法”，因此可以实现更自由、更稳定的绘图。
　此外，该库在绘制后，除非调用更新语法，否则不会重新绘制，因此可以减轻每帧的资源负担。
注意：触摸判断在后台对与设备屏幕尺寸相同的精灵进行单色绘制，因此需要一些内存。
该库基于 M5Unified 制作。
