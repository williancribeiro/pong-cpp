# 🎮 Pong C++ - Jogo em Rede com SFML

Projeto acadêmico da disciplina de **Sistemas Distribuídos** com o objetivo de implementar o clássico jogo Pong utilizando arquitetura **Cliente-Servidor** em **C++**, com comunicação em rede via **protocolo TCP/IP**, e interface gráfica com a biblioteca **SFML**.


## 🏫 Informações do Projeto

- Turma: 1ECP38A1
- Turno: Noite
- Campus: Tijuca

---

## 🕹️ Sobre o Jogo

O Pong é um jogo de tênis de mesa virtual para dois jogadores. Neste projeto, os jogadores podem se conectar via **rede local (LAN)** e disputar partidas em tempo real.

A pontuação é realizada automaticamente, e o jogador com mais pontos ao final do tempo vence.

---

## 🧱 Arquitetura

- Linguagem: **C++**
- Biblioteca gráfica e de rede: **SFML 2.5.1**
- Modelo de rede: **Cliente-Servidor**
- Protocolo: **TCP/IP**

---

## 🛠️ Requisitos

- Sistema operacional: **Windows**
- **SFML 2.5.1** instalada em `C:\SFML-2.5.1`
- **g++** (MinGW ou similar)

---

## ⚙️ Compilação

1. Compile o arquivo `main.cpp`:

```bash
g++ -c main.cpp -I C:\SFML-2.5.1\include
