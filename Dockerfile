FROM        debian:buster

LABEL       authors="tnoulens"

RUN	        apt update && \
            apt upgrade -y && \
            apt install -y curl \
            fish \
            make \
            usermode \
            gcc \
            gcc-multilib \
            valgrind \
            dsniff \
            net-tools \
            tcpdump \
            inetutils-ping && \
            mkdir ft_traceroute && \
            chmod 777 ft_traceroute

WORKDIR     /ft_traceroute/

COPY        . .

RUN        make
