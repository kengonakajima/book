use k;
drop table if exists id_generator;
create table id_generator ( id bigint unsigned );
insert into id_generator set id=1;
drop table if exists Player ;
create table Player (
    id bigint primary key not null auto_increment ,
    accountName char(50) unique, index(accountName) ,
    passwordHash char(50) 
);
drop table if exists PlayerCharacter ;
create table PlayerCharacter (
    id bigint primary key not null auto_increment ,
    playerID bigint, index(playerID) ,
    name char(50), index(name) ,
    level smallint unsigned, index(level) ,
    exp int unsigned ,
    hp int unsigned ,
    maxhp int unsigned ,
    floorID int unsigned ,
    x int unsigned ,
    y int unsigned ,
    equippedItemTypeID int unsigned 
);
drop table if exists CharacterItem ;
create table CharacterItem (
    id bigint primary key not null auto_increment ,
    characterID bigint, index(characterID) ,
    typeID int unsigned ,
    num int unsigned 
);
drop table if exists CharacterSkill ;
create table CharacterSkill (
    id bigint primary key not null auto_increment ,
    characterID bigint, index(characterID) ,
    typeID int unsigned ,
    level int unsigned 
);
drop table if exists PlayerLock ;
create table PlayerLock (
    playerID bigint primary key not null ,
    state tinyint unsigned ,
    ownerServerID int unsigned 
);
