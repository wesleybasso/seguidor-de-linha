#pragma once

static const char PEGASUS_WEB_UI[] PROGMEM = R"HTML(
<!doctype html>
<html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>PEGASUS</title>
  <style>
    body{font-family:Arial,sans-serif;margin:16px;background:#111;color:#eee;padding-top:64px}
    section{margin:12px 0;padding:12px;border:1px solid #333}
    button,input{margin:4px;padding:8px}
    .bar{height:18px;background:#2dd4bf;margin:4px 0}
    .danger{color:#ff7373}
    #topbar{position:fixed;top:0;left:0;right:0;background:#1a1a1a;border-bottom:1px solid #333;
            padding:8px 16px;display:flex;align-items:center;gap:12px;z-index:10}
    #stopbtn{background:#dc2626;color:#fff;border:none;font-weight:bold;font-size:1.1em;padding:10px 24px}
    #link{font-size:0.9em}
    .online{color:#4ade80}.offline{color:#ff7373}
  </style>
</head>
<body>
  <div id="topbar">
    <button id="stopbtn" onclick="cmd('stop')">STOP</button>
    <span>PEGASUS Rev A0</span>
    <span id="link" class="offline">STM32: offline</span>
  </div>
  <section><h2>Status</h2><pre id="status">loading...</pre></section>
  <section><h2>Sensores</h2><div id="sensors"></div></section>
  <section>
    <h2>Comandos</h2>
    <button onclick="cmd('arm')">ARM</button>
    <button onclick="cmd('start')">START</button>
    <button onclick="cmd('stop')">STOP</button>
    <button onclick="cmd('disarm')">DISARM</button>
    <button onclick="cmd('cal_start')">CAL START</button>
    <button onclick="cmd('cal_stop')">CAL STOP</button>
    <button onclick="cmd('ping')">PING</button>
  </section>
  <section>
    <h2>PID</h2>
    <input id="kp" placeholder="Kp"><input id="ki" placeholder="Ki"><input id="kd" placeholder="Kd">
    <button onclick="setPid()">Enviar PID</button>
  </section>
  <section>
    <h2>Config</h2>
    <pre id="config"></pre>
    <button onclick="saveConfig()">Salvar NVS</button>
  </section>
<script>
async function refresh(){
  const t=await (await fetch('/api/status')).json();
  document.getElementById('status').textContent=JSON.stringify(t,null,2);
  const link=document.getElementById('link');
  link.textContent='STM32: '+(t.stm32_online?'online':'offline');
  link.className=t.stm32_online?'online':'offline';
  const s=document.getElementById('sensors'); s.innerHTML='';
  (t.front_norm||[]).forEach((v,i)=>{const d=document.createElement('div');d.textContent='S'+i+' '+v;const b=document.createElement('div');b.className='bar';b.style.width=Math.min(v/10,100)+'%';s.appendChild(d);s.appendChild(b);});
  document.getElementById('config').textContent=await (await fetch('/api/config')).text();
}
async function cmd(c){ await fetch('/api/command?cmd='+c); refresh(); }
async function setPid(){ await fetch('/api/set_pid?kp='+kp.value+'&ki='+ki.value+'&kd='+kd.value); refresh(); }
async function saveConfig(){ await fetch('/api/save_config'); refresh(); }
setInterval(refresh,500); refresh();
</script>
</body>
</html>
)HTML";
