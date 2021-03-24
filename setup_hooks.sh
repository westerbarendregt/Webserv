#!/bin/bash
if (ls $PWD/.git/hooks/pre-push > /dev/null 2>&1);
then
	rm $PWD/.git/hooks/pre-push
fi

if [ "$1" = "--unset" ]
then
	exit 0
fi

ln -s $PWD/hooks/pre-push $PWD/.git/hooks/pre-push
chmod +x $PWD/hooks/pre-push $PWD/.git/hooks/pre-push
