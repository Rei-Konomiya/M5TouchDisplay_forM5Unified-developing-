[EN]
 This library solves the problem of devices with M5Stack touch panels, where drawing and touch judgment must be set or judged separately.
 The library allows for more flexible and stable rendering by making it possible to depend on the parent element for coordinates, colors, etc. as in HTML, and by eliminating the problem of “multiple patterns of syntax with the same behavior,” which is a problem with HTML.
 In addition, this library does not redraw once the drawing is done unless the syntax for updating is called, thus reducing resources per frame.
 Caution: Touch detection requires some memory because monochrome drawing is performed behind the scenes on a sprite the same size as the device screen.
 This library is based on M5Unified.

[JP]
　M5Stackのタッチパネルが存在するデバイスにおいて、描画とタッチ判定をそれぞれ別々で設定または判定しなければならない問題を解決するライブラリです。
　HTMLのように親要素に座標や色などを依存させることが可能であり、かつHTMLで問題となる「同じ動作をする複数パターンの構文」をい統括したため、より自由度と安定性の高い描画を行うことができます。
　また、このライブラリは描画において一度描画した後は更新するための構文が呼び出さない限り再描画を行わないため、フレーム毎のリソースを軽減できます。
　Caution：タッチ判定には裏でデバイスの画面サイズと同じサイズのスプライトにモノクロ描画を行うため、多少のメモリが必要になります。
　このライブラリはM5Unifiedをベースに制作しています。

[CN]
　個函式庫解決了使用 M5Stack 觸控螢幕的裝置，必須分別設定或判斷繪圖與觸控判斷的問題。
　它可以像 HTML 一樣，讓座標、顏色等依賴於父元素，而且由於它可以控制具有相同行為的多種圖案的語法，而這正是 HTML 所面臨的問題，因此它可以讓繪圖具有更大的自由度和穩定性。
　此外，除非在繪圖一次後呼叫更新的語法，否則這個函式庫不會重新繪圖，因此可以減少每一格的資源。
　注意：觸控偵測需要一些記憶體，因為幕後的單色繪圖是在與裝置螢幕大小相同的 sprite 上執行的。
　此程式庫基於 M5Unified。
