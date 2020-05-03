<h1 style="color=red;">internal_relay_chat  :speech_balloon:</h1>
</br>
epic trabalhinho redes

<h2>Descrição</h2>
<p>Implementação dos antigos <strong>Internal Relay Chat (IRC)</strong>, o qual pode ser chamado também de antigo Telegram.</br>O protocolo de comunicação seguido foi o IRC 1459, o qual estabelece um padrão de comunicação client-server.</p>

<h3>Desenvolvido por:</h3>
<p>
Gabriel van Loon Bode da Costa Dourado Fuentes Rojas</br>
Mathias Fernandes Duarte Coelho</br>
joao Ricardo Minoru Nagasava</br>
</p>

## Compilando e Executando
Em <a href="/client_src">client_src</a> e <a href="/server_src">server_src</a> contém melhores explicações de como proceder seguramente,
Ou para facilitar, utilize os comandos `make server` `make server_run` e  `make client` `make client_run` a partir da pasta root.

## Nas pastas...
<ul>
	<li><a href="/client_src">client_src</a> executaveis, esquemas e README documentando-os. O "makeClient" facilita o teste de clientes.</li>
	<li><a href="/server_src">server_src</a> executaveis, fluxograma e README documentando-os.</li>
	<li><a href="/assets">assets</a> Todas imagens e arquivos visuais.</li>
	<li><a href="/socket_example">socket_example</a> Outras versões feitas para testes apenas.</li>
</ul>
<p>A seguir demo e imagens <strong>inéditas</strong> de testes e dados.</p>
<h2>Demo</h2>
<div style="display:inline-block;">
	<img src="assets/primeiro_client.png" alt="client"></img>
	<img src="assets/segundo_client.png" alt="client"></img>
	<img src="assets/terceiro_client.png" alt="client"></img>
</div>

<h2>Testando com htop</h2>
<p>Obs: o status R(Running) e S(Sleeping)</p>
<p>Nas imagens é possível ver CPU, MEM e qual thread está correndo. Podemos ver resultados otimistas.</p>
<div style="display:inline-block;">
	<img src="assets/threads_client.png" alt="client"></img>
	<img src="assets/threads_server.png" alt="client"></img>
</div>

<h2>Environment testados</h2>
<p>OS: Fedora 30.x</br>
gcc version 9.2.1 20190827 (Red Hat 9.2.1-1)</br> 
g++ (GCC) 9.2.1 20190827 (Red Hat 9.2.1-1)</br>
</p>
<p>OS: Ubuntu 18.04.4 LTS x86_64</br>
Shell: bash 4.4.20</br>
g++ (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0</br>
</p>
