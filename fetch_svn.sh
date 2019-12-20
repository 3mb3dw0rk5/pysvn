#!/bin/bash

current_branch=$(git branch --show-current)
git checkout downstream
git svn fetch
git checkout $current_branch
