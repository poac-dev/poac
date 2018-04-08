## TODO
* **パッケージ名は，-(ハイフン)禁止 -> _ (アンダーバー)を推奨．（そのままnamespaceになるから）**

* depsを，githubからもインストール可能にする．

* https://hex.pm/docs/publish
* https://qiita.com/ma2ge/items/0e19bf3f03078f589096
* http://blog.tokoyax.com/entry/elixir/register-to-hex
* ログイン関係の機構に関してはここを参考にしたい．

* publish時に，登録時のパスフレーズを入力する．しかし，そもそもregisterでアカウント作成していない場合，API Keyがmixの管理下に存在しないため，使えない．

* 別PCで作成したアカウントにログインのみしたい場合，registerコマンドで，ログインのみができるのか？？
* それとも，API Keyを移行する必要があるのか？？？
* それとも，publish時？？
* No: API Keyを，~/.poac/に移行するか，poac signinコマンドでパスワードとユーザーIDを入力すれば，同じAPI Keyが作成されます．
