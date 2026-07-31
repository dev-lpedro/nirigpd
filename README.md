# niripad (v0.1)

**niripad** é um daemon em C++ e SDL3 para mapear controles/gamepads para ações no compositor Wayland [Niri](https://github.com/YaLteR/niri).

Ele permite criar atalhos globais, combos e modos de navegação utilizando botões e eixos analógicos do seu controle.

---

## 🛠️ Requisitos
* C++20
* SDL3
* magic_enum
* kdl-c

---

## 📦 Como Compilar e Rodar

```bash
# Clone o repositório
git clone [https://github.com/dev-lpedro/niripad.git](https://github.com/dev-lpedro/niripad.git)
cd niripad

# Copie o arquivo de configuração de exemplo
cp gamepad.kdl.example gamepad.kdl

# Compile
mkdir build && cd build
cmake .. && make

# Execute
./niripad
```

🎯 Roadmap & Futuras Implementações

Esta versão v0.1 é um protótipo funcional do núcleo do programa. As seguintes melhorias estão planejadas para as próximas versões:

  [ ] Reescrita do Parser KDL: Para aproximar a sintaxe do arquivo de configuração ao padrão nativo do Niri.
  [ ] Hot-Reload de Configuração: Atualização automática das bindings ao salvar o arquivo .kdl sem precisar reiniciar o processo.
  [ ] Modo Daemon & Integração Systemd: Criação do arquivo niripad.service para gerenciamento nativo de background via systemd --user.
  [ ] Hot-plugging: Reconexão automática do gamepad em caso de desconexão do cabo ou bluetooth.

📋 Nota de Desenvolvimento

  Nesta versão v0.1, a lógica do parser KDL foi gerada com auxílio de IA para agilizar o protótipo e permitir focar o aprendizado na integração com a biblioteca SDL3. Uma reescrita manual em C++ está planejada para a v0.2.
  Projeto inspirado pela experiência de navegação do compositor Niri.

📄 Licença

Este projeto está sob a licença MIT - veja o arquivo LICENSE para mais detalhes.
