# -*- coding: utf-8; -*-    

# ▼概要
# このスクリプトは、K Onlineのテーブル構造を定義し、
# そのテーブルをgmsvから非同期的に利用するための役割を果たす
# dbsvプロセスを実装する、すべてのC++コードを生成するツールです。
# なおこのツールは、 CREATE TABLE文を含むSQLファイルや、
# VCEのgen.exe用の定義XMLファイルも自動生成します。
# K Onlineのgmsvにおいては、gen.exeを用いてこのXMLファイルから
# dbproto.[h|cpp]　をさらに生成してコンパイル・リンクして用います。
# このツールは、Django, MySQL　に依存しています。
#
# 実際のコード生成には、
#  - dbgen.py
#  - svtemplate.[cpp|h]
#  - cltemplate.[cpp|h]
#  - template.xml
#  - template.sql
# 以上のファイルをテキストテンプレートとして使います。
#

#
# ▼各クラスの説明
#  - Database  ジェネレータ全体に必要な情報を保持します。
#   - name : SQLにおけるデータベースの名称を指定します。
#   - version : 定義ファイルのバージョン番号を指定します。(未使用)
#  - Table  テーブルの設定を保持します。
#   - name : テーブルの名称を指定します。この名称は構造体の名称にもなります。
#  - Field  テーブルの各フィールドの設定を保持します。
#   - name : フィールドの名称を文字列で指定します。構造体のメンバ名もこれに等しくなります。
#   - type : フィールドの型を、VCEのgenの形式で指定します。char/short/int, byte/word/dword/qword, string のいずれかを指定します。
#   - size : typeがstringの場合は文字列の最大長を、それ以外の場合は配列の最大長を指定します。
#   - primary : 指定したフィールドが、テーブルのprimary keyとなります。1個だけ指定できあす。
#   - auto_increment : 指定したフィールドに auto_incrementが適用されます。1個だけ指定できます。
#   - index : 指定したフィールドに、indexを設定します。複数フィールドに対して指定できます。
#   - unique : 指定したフィールドに、unique属性を設定します。複数フィールドに対して指定できます。
#


from dbgen import Database, Table, Field

#
# 以下、DBテーブル構造定義を行う
#

#
# 利用するデータベース名を指定します。
#

db = Database(name="k", version=1 )

# ▼Playerテーブル. プレイヤーの情報を保存するテーブルを定義します。

tbl = Table(name="Player")
tbl.add( Field(name="id", type="qword", primary = True, auto_increment = True)) # 通し番号
tbl.add( Field( name="accountName", type="string", size=50, index=True, unique=True )) # 排他するログイン時に入力する名前。
tbl.add( Field( name="passwordHash", type="string", size=50 )) # パスワードのハッシュ値
db.add(tbl)

tbl = Table(name="PlayerCharacter")
tbl.add( Field( name="id", type="qword", primary = True, auto_increment=True))
tbl.add( Field( name="playerID", type="qword", index=True ))
tbl.add( Field( name="name", type="string", size=50, index=True)) # 他のプレイヤーに見せるキャラクターの名前。排他しない。
tbl.add( Field( name="level", type="word", index=True ))  # キャラクターのレベル
tbl.add( Field( name="exp", type="dword" )) # キャラクターの経験値
tbl.add( Field( name="hp", type="dword" ))  # 現在HP
tbl.add( Field( name="maxhp", type="dword" )) # 現在MAXHP
tbl.add( Field( name="floorID", type="dword" )) # どのフロアにいるか
tbl.add( Field( name="x", type="dword" ))  # マップ上のどの位置にいるか
tbl.add( Field( name="y", type="dword" ))
tbl.add( Field( name="equippedItemTypeID", type="dword" ))   #装備してるアイテムのタイプ
db.add(tbl)

# ▼CharacterItemテーブル。 charが保持しているアイテムの情報を保存するテーブルです。
# 一人のcharあたりアイテムの種類の数と同じ行数を消費します。
tbl = Table(name="CharacterItem")
tbl.add( Field( name="id", type="qword", primary=True, auto_increment=True ))
tbl.add( Field( name="characterID", type="qword", index=True ))
tbl.add( Field( name="typeID", type="dword"))
tbl.add( Field( name="num", type="dword")) # 消費して無くなったら0を書き込む
db.add(tbl)

# ▼Skillテーブル。
tbl = Table( name="CharacterSkill" )
tbl.add( Field( name="id", type="qword", primary=True, auto_increment=True ))
tbl.add( Field( name="characterID", type="qword", index=True ))
tbl.add( Field( name="typeID", type="dword"))
tbl.add( Field( name="level", type="dword")) 
db.add(tbl)

# ▼Lockテーブル。

tbl = Table( name="PlayerLock" )
tbl.add( Field( name="playerID", type="qword", primary=True ))
tbl.add( Field( name="state", type="byte", conditional=True ))
tbl.add( Field( name="ownerServerID", type="dword" ))
db.add(tbl)




#
# 以下、実際にファイル出力
#
import codecs

# SQLのテーブルを生成するためのスクリプトを出力
codecs.open( "k_table_create.sql","w", "utf-8" ).write(
    db.create_sql_source( drop = True, create = True ) )
# VCE gen用の定義ファイルを出力
codecs.open("dbproto.xml", "w", "utf-8").write(
    db.create_proto_xml())
# dbsvの本体ソースを出力
codecs.open("dbsvmain.h", "w", "utf-8").write(
    db.create_sv_h())
codecs.open("dbsvmain.cpp", "w", "utf-8").write(
    db.create_sv_cpp())
# dbsvを単体テストするための自動テスタを出力
codecs.open("dbclmain.h", "w", "utf-8").write(
    db.create_cl_h())
codecs.open("dbclmain.cpp", "w", "utf-8").write(
    db.create_cl_cpp())


