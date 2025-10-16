FROM ubuntu:latest

# 设置非交互模式
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        git \
        # 清理 APT 缓存以减小镜像体积
        ca-certificates && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app

CMD ["bash"]