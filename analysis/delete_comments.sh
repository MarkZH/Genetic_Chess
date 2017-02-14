#!/bin/bash

# Delete curly-braced enclosed commentary and extraneous whitespaces
sed -e 's/{[^}]*}//g' -e 's/\s\+/ /g' -e 's/\s\+$//'
