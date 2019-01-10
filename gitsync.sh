#!/bin/bash
#
# https://help.github.com/articles/syncing-a-fork/
# https://help.github.com/articles/configuring-a-remote-for-a-fork/
# https://www.opentechguides.com/how-to/article/git/177/git-sync-repos.html
#
# Setup:
#
# git remote add --mirror=fetch secondary https://secondary_repo_url/secondary_repo.git
# git fetch origin
# git push secondary --all
#
#

git push

git fetch origin
git checkout master
git merge origin/master

for repo in $(git remote -v | grep -v origin | grep "(push)" | awk '{print $1}')
do
	echo "Updating ${repo} ..."
	git push ${repo}
done


