## TODO
外部依存など！！！
* bashのバージョンは4以上．


**/usr/local/binにリンクを貼る**
**poac-listコマンド廃止・理由：poac.yamlを見れば良い**
**パッケージ名は，-(ハイフン)禁止 -> _ (アンダーバー)を推奨．（そのままnamespaceになるから）**

manページを書く
man hoge で，manページが開かれる機構は，システム上のmanディレクトリに
リンクを貼るとかしなくても大丈夫なものなのか？？？

makefileで，ln -s などをする．

depsを，githubからもインストール可能にする．

poac initで，既にファイルが存在する場合，かつ，パッケージをインストールしていたら，完全に初期化上書きしてしまうと，depsフォルダは存在するのに，なにもインストールしていないことになってしまうため，npmのように，依存関係を見て，完全に初期化しないようにするか，
こっちにする！！！だって，初期化コマンドだから！！！！！！！！http://hoge.com
初期化する場合，warningにもdepsディレクトリ丸ごと消す旨を記載したのちに，削除するかのどちらかをする．

yamlの文法！！！
http://yaml.org/spec/current.html
http://magazine.rubyist.net/?0009-YAML
yamlパーサーのみ，ヘッダーオンリーライブラリ（と，Boostのパーサー）で作りたい．
(相互変換？？？？？)
Bashの変数に変換し，それの標準出力を取得する．
つまり，input output のインターフェースは変化しないということ．(inputを変えずにできるかはわからない)
C++の変数に変換するのならば，一つの変数に展開することになる．
Pythonなら，
```python
>>> yaml = input('poac.yaml')
>>> print(yaml)
dict
{
	app: hoge,
	...
}
```
となっているからである．
現行の，ヘッダーオンリーでない(ビルドが必要な)yamlパーサーがどうしているかリサーチする必要がある．

フィードバックをもらえるところがあるとよい．

install のみで，引数がなければ，カレントディレクトリに，poac.jsonが存在するか確認．
もし存在していれば，そこに記載されたdeps(依存関係を全てインストール(もし，depsディレクトリが存在する場合，差分でインストールを行う．しかし，バージョンが新しいものが存在する場合，アップデートを行う？もしくは，確認を取ってから判断する．))

https://hex.pm/docs/publish
https://qiita.com/ma2ge/items/0e19bf3f03078f589096
http://blog.tokoyax.com/entry/elixir/register-to-hex
ログイン関係の機構に関してはここを参考にしたい．

publish時に，登録時のパスフレーズを入力する．しかし，そもそもregisterでアカウント作成していない場合，API Keyがmixの管理下に存在しないため，使えない．

別PCで作成したアカウントにログインのみしたい場合，registerコマンドで，ログインのみができるのか？？
それとも，API Keyを移行する必要があるのか？？？
それとも，publish時？？
No: API Keyを，~/.poac/に以降するか，poac signinコマンドでパスワードとユーザーIDを入力すれば，同じAPI Keyが作成されます．

https://www.google.co.jp/search?tbm=isch&q=poach
アイコンはこれをイラスト化？？？
（ただし，poach自体には，奪うや引き抜くなどのbadなイメージがあるので，それに繋げない方が良い？？）

libyamlに依存する！！
で，homebrewでのインストール時などに，gcc -lyaml main.cなどでコンパイルしておく！

