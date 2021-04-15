set -x
[[ "$(curl -s --resolve name1:8080:127.0.0.1 http://name1:8080)" == "name1" ]] || exit 1
[[ "$(curl -s --resolve name2:8080:127.0.0.1 http://name2:8080)" == "name2" ]] || exit 1
[[ "$(curl -s --resolve name3:8080:127.0.0.1 http://name3:8080)" == "name3" ]] || exit 1
[[ "$(curl -s --resolve hello:8080:127.0.0.1 http://hello:8080)" == "name1" ]] || exit 1
