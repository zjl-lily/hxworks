#!/bin/bash
# Author: hxsl
# Created Time: 2022年01月20日 星期四 11时14分43秒

repo=$1
project=$2

git init
git add ./
git commit -m "init $repo/$project"
git remote add $project git@github.com:zjl-lily/$repo
git push -u  $project  master



