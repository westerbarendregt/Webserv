FROM debian:buster

RUN apt-get update && \
	apt-get -y upgrade && \
	apt-get -y install mariadb-server mariadb-client && \
	apt-get -y install php-mysql php-mbstring php-zip php-gd php-xml php-pear php-gettext php-cgi && \
	apt-get -y install wget && \
	apt-get -y install curl && \
	apt-get -y install libnss3-tools && \
    apt-get -y install build-essential --no-install-recommends && \
    apt-get -y install clang

RUN mkdir -p /webserv/www && \
    mkdir /webserv/www/wordpress && \
    mkdir /webserv/www/phpmyadmin && \
    mkdir /webserv/conf

WORKDIR /webserv/

COPY ft_server/srcs/wp-config.php www/wordpress/
COPY ft_server/srcs/config.inc.php www/phpmyadmin/

RUN service mysql start && \
	mysql -e "CREATE DATABASE wordpress;" && \
	mysql -e "CREATE USER 'admin'@'localhost' IDENTIFIED BY 'cheese';" && \
	mysql -e "GRANT ALL PRIVILEGES ON * . * TO 'admin'@'localhost';" && \
	mysql -e "FLUSH PRIVILEGES;"

RUN wget https://files.phpmyadmin.net/phpMyAdmin/5.0.1/phpMyAdmin-5.0.1-english.tar.gz -P /tmp && \
	tar -xzvf /tmp/phpMyAdmin-5.0.1-english.tar.gz --strip-components=1 -C /webserv/www/phpmyadmin && \
	mkdir /webserv/www/phpmyadmin/tmp && \
	chmod 777 /webserv/www/phpmyadmin/tmp

RUN wget https://wordpress.org/latest.tar.gz -P /tmp && \
	tar -xzf /tmp/latest.tar.gz --strip-components=1 -C /webserv/www/wordpress/ && \
	wget -O wp https://raw.githubusercontent.com/wp-cli/builds/gh-pages/phar/wp-cli.phar && \
	chmod +x wp && \
	mv wp /usr/local/bin/ && \
	service mysql restart && \
	wp core install --allow-root --url=localhost --path=/webserv/www/wordpress --title="Webserv" --admin_user=admin --admin_password=cheese --admin_email=bpeeters@student.codam.nl --skip-email

COPY Makefile .
COPY srcs/ srcs/
COPY includes/ includes/
COPY conf_templates/ft_server.conf conf/

RUN make LOG=0

EXPOSE 80

CMD ["bash", "-c", "service mysql start && ./webserv conf/ft_server.conf"]
