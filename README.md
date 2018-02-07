## TODO
外部依存など！！！
* bashのバージョンは4以上．

manページを書く

brew install 可能にする
apt-getやyumなども．

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

install のみで，引数がなければ，カレントディレクトリに，poac.jsonが存在するか確認．
もし存在していれば，そこに記載されたdeps(依存関係を全てインストール(もし，depsディレクトリが存在する場合，差分でインストールを行う．しかし，バージョンが新しいものが存在する場合，アップデートを行う？もしくは，確認を取ってから判断する．))

https://hex.pm/docs/publish
https://qiita.com/ma2ge/items/0e19bf3f03078f589096
http://blog.tokoyax.com/entry/elixir/register-to-hex
ログイン関係の機構に関してはここを参考にしたい．

publish時に，登録時のパスフレーズを入力する．しかし，そもそもregisterでアカウント作成していない場合，API Keyがmixの管理下に存在しないため，使えない．

疑問点 => 別PCで作成したアカウントにログインのみしたい場合，registerコマンドで，ログインのみができるのか？？
それとも，API Keyを移行する必要があるのか？？？
それとも，publish時？？

https://www.google.co.jp/search?tbm=isch&q=poach
アイコンはこれをイラスト化？？？
（ただし，poach自体には，奪うや引き抜くなどのbadなイメージがあるので，それに繋げない方が良い？？）

https://ubiteku.oinker.me/2016/02/08/elixir-phoenix-auto-hot-deploy/
Exrmは，これを参考？？？
