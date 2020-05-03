<h1 style="color=red;">internal_relay_chat</h1>
</br>
epic trabalhinho redes

<h2>Descrição</h2>
<p>Implementação dos antigos <strong>Internal Relay Chat (IRC)</strong>, o qual pode ser chamado também de antigo Telegram.</p>
</br>
<p>O protocolo de comunicação seguido foi o IRC 1459, o qual estabelece um padrão de comunicação client-server.</p>
<p>A seguir demo, fluxograma, testes e dados para <strong>executar a aplicação.</strong>. 

<h2>Demo</h2>

<h2>Fluxograma Geral do servidor</h2>
<p align="center">
	<img src="/server_socket.png" width="50%" heigth="50%" alt="fluxograma_do_server"></img>
</p>
<h2>Testando com htop</h2>
<p>Obs: o status R(Running) e S(Stop)</p>

<h2>Running...<h2>
</br>
<p>Com Makefile:</p>
</br>

```
  cd internal_relay_chat/server_src/
  make all
  make run
  cd internal_relay_chat/client_src/
  make all
  make run
```

<h2>Environment testados</h2>
<p>gcc version 9.2.1 20190827 (Red Hat 9.2.1-1)</br> 
g++ (GCC) 9.2.1 20190827 (Red Hat 9.2.1-1)</br>
</p>
