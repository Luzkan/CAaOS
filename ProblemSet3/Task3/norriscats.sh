cat=$(curl -s https://api.thecatapi.com/v1/images/search?format=json | jq -r '.[0].url')
wget $cat -O /tmp/img.jpg
img2txt /tmp/img.jpg > cat.txt
cat cat.txt

curl -s http://api.icndb.com/jokes/random/ | jq '.value.joke' 

