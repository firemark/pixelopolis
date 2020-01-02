#!/bin/bash
set -e
heroku container:login
HEROKU_TOKEN=$(heroku auth:token)
HEROKU_IMAGE=registry.heroku.com/pixelopolis/web:latest
docker build . -t $HEROKU_IMAGE
docker login --username=_ --password=$HEROKU_TOKEN registry.heroku.com
docker push $HEROKU_IMAGE
HEROKU_URL=https://api.heroku.com/apps/pixelopolis/formation
DOCKER_ID=$(docker inspect $HEROKU_IMAGE --format={{.Id}})
curl -n -X PATCH $HEROKU_URL \
    -d "{\"updates\":[{\"type\": \"web\", \"docker_image\": \"$DOCKER_ID\"}]}" \
    -H "Content-Type: application/json" \
    -H "Accept: application/vnd.heroku+json; version=3.docker-releases" \
    -H "Authorization: Bearer $HEROKU_TOKEN"
