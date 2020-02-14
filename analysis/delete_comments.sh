#!/bin/bash

# Delete curly-braced enclosed commentary and extraneous whitespaces
sed -e 's/([^()]*)//g' -e 's/([^()]*)//g' -e 's/{[^}]*}//g' -e 's/;.*//' -e 's/\s\+/ /g' -e 's/\s\+$//'
#       ^                  ^                  ^                 ^            ^               ^
#       ^                  ^                  ^                 ^            ^               Delete trailing spaces
#       ^                  ^                  ^                 ^            Consolidate spaces
#       ^>>>>>>>>>>>>>>>>>>^                  ^                 Delete after semicolon
#       ^                                     Delete comments between braces
#       Delete variations between parentheses
#       (max two levels of nesting)
