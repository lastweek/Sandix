if [ -x "$$BASH" ];
	then echo $$BASH;
else if [ -x /bin/bash ];
	then echo /bin/bash;
else echo sh; fi ; fi
