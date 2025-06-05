# hdd-music-web
## 環境構築
環境構築の方法
ubuntu、macOSを前提として進めます
windowsは公式を見てください https://mise.jdx.dev/getting-started.html
### 1. miseのインストール
``` bash
curl https://mise.run | sh
```
インストールの確認
```
~/.local/bin/mise --version
# mise 2024.x.x
```
あとは、パスを指定せずにmiseが使えるはずです。

### 2. miseのアクティブ化
これを行うと、`node`や`go`といったコマンドを`mise exec -- <command>`を使用せずに使えるようになります。
``` bash
echo 'eval "$(~/.local/bin/mise activate bash)"' >> ~/.bashrc
```

### 3. 開発環境のインストール
以下のコマンドを実行すればOKです
```
mise install
```

## toolの追加
`go`や`node`、`Python`といった環境の追加は以下のように行います
``` bash
mise use go@1.24
```
これで新しいtoolを追加できます。
それぞれの言語でのpackage追加はそれぞれに基づいてやってみてください。
(尚、pythonはpoetryで行います)
