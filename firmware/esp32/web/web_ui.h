#pragma once

static const char PEGASUS_WEB_UI[] PROGMEM = R"HTML(
<!doctype html>
<html lang="pt-BR">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width,initial-scale=1">
  <title>PEGASUS</title>
  <style>
    :root{--bg:#080808;--p:#111;--p2:#181818;--p3:#222;--line:#f4f4f4;--muted:#a7a7a7;--orange:#ff7a18;--danger:#ff4d2f;--ok:#42d66b;--warn:#ffd166;--r:8px;color-scheme:dark}
    *{box-sizing:border-box}
    body{margin:0;min-width:320px;background:linear-gradient(90deg,rgba(255,122,24,.05) 1px,transparent 1px),linear-gradient(0deg,rgba(255,255,255,.035) 1px,transparent 1px),var(--bg);background-size:48px 48px;color:var(--line);font-family:Arial,system-ui,sans-serif;letter-spacing:0}
    button,input{font:inherit}
    button{border:1px solid #3a3a3a;border-radius:7px;background:var(--p3);color:var(--line);min-height:42px;padding:0 12px;font-weight:800}
    button:focus-visible,input:focus-visible{outline:2px solid var(--orange);outline-offset:2px}
    button:disabled{opacity:.42}
    input{width:100%;border:1px solid #333;border-radius:7px;background:#090909;color:var(--line);min-height:38px;padding:0 10px}
    #topbar{position:sticky;top:0;z-index:10;display:grid;grid-template-columns:minmax(130px,1fr) auto auto auto;gap:10px;align-items:center;padding:10px 14px;background:rgba(8,8,8,.95);border-bottom:1px solid #333}
    .brand strong{display:block;font-size:1rem}.brand span{display:block;color:var(--muted);font-size:.75rem}
    #state{font-weight:900;color:var(--ok);white-space:nowrap}
    #clock{font-variant-numeric:tabular-nums;color:var(--muted);white-space:nowrap}
    #stopbtn{min-height:52px;padding:0 24px;background:var(--danger);color:#080808;border:2px solid #fff;font-size:1.05rem;box-shadow:0 0 0 4px rgba(255,77,47,.16)}
    .shell{width:min(1280px,100%);margin:auto;padding:14px;display:grid;gap:14px}
    .alert{display:grid;grid-template-columns:1fr auto;gap:10px;align-items:center;border:1px solid rgba(255,122,24,.55);border-radius:var(--r);background:linear-gradient(90deg,rgba(255,122,24,.16),rgba(17,17,17,.92));padding:10px}
    .alert strong{display:block}.alert span{color:var(--muted);font-size:.88rem}
    .grid{display:grid;grid-template-columns:minmax(0,1.35fr) minmax(320px,.85fr);gap:14px;align-items:start}
    .stack{display:grid;gap:14px}
    .panel{border:1px solid #333;border-radius:var(--r);background:rgba(17,17,17,.96);box-shadow:0 12px 28px rgba(0,0,0,.32);overflow:hidden}
    .ph{display:flex;justify-content:space-between;gap:8px;align-items:center;padding:10px 12px;border-bottom:1px solid #2d2d2d;background:linear-gradient(180deg,#181818,#111)}
    h2{margin:0;font-size:.92rem}.sub{color:var(--muted);font-size:.76rem;text-align:right}
    .body{padding:12px}
    .metrics{display:grid;grid-template-columns:repeat(4,minmax(0,1fr));gap:10px}
    .metric{border:1px solid #303030;border-radius:var(--r);background:var(--p2);padding:10px;min-width:0}
    .metric label{display:block;color:var(--muted);font-size:.68rem;text-transform:uppercase}
    .metric strong{display:block;margin-top:4px;font-size:1.24rem;font-variant-numeric:tabular-nums;white-space:nowrap}
    .metric small{color:var(--muted)}
    .ok{color:var(--ok)!important}.danger{color:var(--danger)!important}.warn{color:var(--warn)!important}.accent{color:var(--orange)!important}
    .track{position:relative;height:88px;border:1px solid #333;border-radius:var(--r);background:#0a0a0a;overflow:hidden}
    .track:before{content:"";position:absolute;left:18px;right:18px;top:40px;height:8px;background:#fff;border-radius:999px;transform:translateX(var(--line-x,0px));box-shadow:0 0 18px rgba(255,255,255,.2)}
    .track:after{content:"";position:absolute;left:50%;top:12px;width:2px;height:64px;background:var(--orange);box-shadow:0 0 14px var(--orange)}
    .track .car{position:absolute;left:50%;top:24px;width:56px;height:36px;margin-left:-28px;border:2px solid var(--orange);border-radius:7px;background:rgba(255,122,24,.08)}
    .track .car:before,.track .car:after{content:"";position:absolute;top:8px;width:6px;height:20px;border-radius:3px;background:#ddd}.track .car:before{left:6px}.track .car:after{right:6px}
    .sensors{display:grid;grid-template-columns:repeat(8,minmax(26px,1fr));gap:8px}
    .sensor{text-align:center;font-size:.76rem}.bar{height:112px;background:#080808;border:1px solid #333;border-radius:6px;position:relative;overflow:hidden}.fill{position:absolute;left:0;right:0;bottom:0;height:var(--h);background:linear-gradient(0deg,var(--orange),#fff)}
    .lat{display:grid;grid-template-columns:repeat(2,1fr);gap:8px;margin-top:10px}.lat div,.health div{display:flex;justify-content:space-between;gap:8px;border:1px solid #303030;border-radius:7px;background:var(--p2);padding:8px}
    .led{width:10px;height:10px;border-radius:50%;background:var(--ok);box-shadow:0 0 14px currentColor}.off{background:#555;box-shadow:none}
    .actions{display:grid;grid-template-columns:repeat(2,1fr);gap:8px}.primary{background:var(--orange);color:#090909}.soft-danger{border-color:rgba(255,77,47,.6);color:#ffb4a8}
    .pid{display:grid;grid-template-columns:repeat(3,1fr);gap:8px}.full{grid-column:1/-1}
    .raw{max-height:130px;overflow:auto;margin:0;padding:10px;border-radius:7px;background:#080808;color:#ccc;font-size:.78rem}
    @media(max-width:900px){#topbar{grid-template-columns:1fr auto}.grid,.metrics,.pid{grid-template-columns:1fr}.sensors{grid-template-columns:repeat(4,1fr)}#clock{display:none}#stopbtn{grid-column:1/-1}}
  </style>
</head>
<body>
  <header id="topbar">
    <div class="brand"><strong>PEGASUS Rev A0</strong><span id="link">STM32 offline</span></div>
    <span id="state">BOOT</span>
    <span id="clock">00:00.000</span>
    <button id="stopbtn" data-cmd="stop" type="button">STOP</button>
  </header>
  <main class="shell">
    <section class="alert"><div><strong id="gate">Fase 0 - bancada sem motores</strong><span>STOP sempre disponivel. Motores/succao reais exigem teste suspenso e fonte limitada.</span></div><button data-cmd="ping" type="button">PING</button></section>
    <div class="grid">
      <div class="stack">
        <section class="panel"><div class="ph"><h2>Telemetria</h2><span class="sub">25 Hz alvo</span></div><div class="body metrics">
          <div class="metric"><label>Bateria</label><strong id="bat">0.00 V</strong><small id="batNote">pendente</small></div>
          <div class="metric"><label>Erro PID</label><strong id="piderr">0</strong><small>centro = 0</small></div>
          <div class="metric"><label>Posicao</label><strong id="pos">0</strong><small>linha</small></div>
          <div class="metric"><label>Tempo</label><strong id="run">00:00.000</strong><small>run</small></div>
        </div></section>
        <section class="panel"><div class="ph"><h2>Linha</h2><span class="sub" id="lineFlags">aguardando</span></div><div class="body">
          <div class="track" id="track"><span class="car"></span></div>
          <div class="lat"><div><span><span class="led off" id="latLled"></span> LAT_L</span><strong id="latL">0</strong></div><div><span><span class="led off" id="latRled"></span> LAT_R</span><strong id="latR">0</strong></div></div>
        </div></section>
        <section class="panel"><div class="ph"><h2>Sensores</h2><span class="sub">normalizado 0-1000</span></div><div class="body"><div class="sensors" id="sensors"></div></div></section>
      </div>
      <div class="stack">
        <section class="panel"><div class="ph"><h2>Comandos</h2><span class="sub" id="lastcmd">idle</span></div><div class="body actions">
          <button class="primary" data-cmd="arm" type="button">ARM</button>
          <button class="primary" data-cmd="start" type="button">START</button>
          <button data-cmd="disarm" type="button">DISARM</button>
          <button data-cmd="cal_start" type="button">CAL START</button>
          <button data-cmd="cal_stop" type="button">CAL STOP</button>
          <button class="soft-danger" data-cmd="hw_test" type="button">HW TEST</button>
        </div></section>
        <section class="panel"><div class="ph"><h2>Saidas</h2><span class="sub">PWM</span></div><div class="body metrics">
          <div class="metric"><label>Motor L</label><strong id="pwmL">0</strong><small>PB8</small></div>
          <div class="metric"><label>Motor R</label><strong id="pwmR">0</strong><small>PB9</small></div>
          <div class="metric"><label>Fan</label><strong id="fan">0</strong><small>PA8</small></div>
          <div class="metric"><label>Link</label><strong id="online" class="danger">OFF</strong><small>UART</small></div>
        </div></section>
        <section class="panel"><div class="ph"><h2>PID</h2><span class="sub">perfil local</span></div><div class="body pid">
          <input id="kp" placeholder="Kp"><input id="ki" placeholder="Ki"><input id="kd" placeholder="Kd">
          <button class="primary full" id="sendPid" type="button">Enviar PID</button>
        </div></section>
        <section class="panel"><div class="ph"><h2>Sistema</h2><span class="sub">config</span></div><div class="body health">
          <div><span>STM32 UART</span><strong id="uartHealth" class="danger">offline</strong></div>
          <div><span>Error flags</span><strong id="errors">0x00000000</strong></div>
          <button id="save" type="button">Salvar NVS</button>
          <pre id="config" class="raw"></pre>
        </div></section>
      </div>
    </div>
  </main>
<script>
const $=id=>document.getElementById(id);
const state=$('state'),link=$('link'),clock=$('clock'),gate=$('gate'),track=$('track'),sensors=$('sensors');
function time(ms){ms=ms||0;const m=Math.floor(ms/60000),s=Math.floor(ms%60000/1000),x=ms%1000;return String(m).padStart(2,'0')+':'+String(s).padStart(2,'0')+'.'+String(x).padStart(3,'0')}
function cls(el,name){el.className=name}
async function cmd(c){$('lastcmd').textContent=c+'...';try{const r=await fetch('/api/command?cmd='+encodeURIComponent(c));$('lastcmd').textContent=c+': '+await r.text()}catch(e){$('lastcmd').textContent=c+': erro'}}
async function refresh(){
  try{
    const t=await (await fetch('/api/status')).json();
    const bad=t.state==='SAFE_STOP'||t.state==='ERROR'||t.state==='LINE_LOST'||t.line_lost;
    state.textContent=t.state||'UNKNOWN';cls(state,bad?'danger':(t.state==='RUNNING'?'ok':'warn'));
    const age=t.last_update_age_ms||0;
    link.textContent='STM32 '+(t.stm32_online?'online':'offline')+(t.stm32_online?' '+age+'ms':'');$('online').textContent=t.stm32_online?'ON':'OFF';cls($('online'),t.stm32_online?'ok':'danger');$('uartHealth').textContent=t.stm32_online?'online '+age+'ms':'offline';cls($('uartHealth'),t.stm32_online?'ok':'danger');
    clock.textContent=time(t.run_time_ms);$('run').textContent=time(t.run_time_ms);
    $('bat').textContent=((t.battery_mv||0)/1000).toFixed(2)+' V';$('batNote').textContent=t.battery_mv?'medida':'pendente';
    $('piderr').textContent=(t.pid_error>0?'+':'')+(t.pid_error||0);$('pos').textContent=t.line_position||0;
    $('pwmL').textContent=t.pwm_left||0;$('pwmR').textContent=t.pwm_right||0;$('fan').textContent=t.fan_pwm||0;
    $('errors').textContent='0x'+String((t.error_flags||0).toString(16)).padStart(8,'0');
    $('lineFlags').textContent=(t.line_detected?'LINE ':'NO LINE ')+(t.crossing_detected?'CROSS ':'')+(t.line_lost?'LOST':'');
    track.style.setProperty('--line-x',Math.max(-38,Math.min(38,(t.line_position||0)/20))+'px');
    $('latL').textContent=t.lat_l_norm||0;$('latR').textContent=t.lat_r_norm||0;cls($('latLled'),'led '+(t.lat_l_active?'':'off'));cls($('latRled'),'led '+(t.lat_r_active?'':'off'));
    sensors.innerHTML='';(t.front_norm||[]).forEach((v,i)=>{const d=document.createElement('div');d.className='sensor';d.innerHTML='<div class="bar"><div class="fill" style="--h:'+Math.min(v/10,100)+'%"></div></div><span>S'+i+'</span><strong>'+v+'</strong>';sensors.appendChild(d)});
    const canStart=t.state==='ARMED'||t.state==='READY';document.querySelector('[data-cmd=start]').disabled=!canStart;gate.textContent=t.stm32_online?'Fase 0 - link ativo':'Fase 0 - aguardando STM32';
    $('config').textContent=await (await fetch('/api/config')).text();
  }catch(e){link.textContent='ESP32 API erro';cls(state,'danger')}
}
document.querySelectorAll('[data-cmd]').forEach(b=>b.addEventListener('click',()=>cmd(b.dataset.cmd).then(refresh)));
$('sendPid').addEventListener('click',()=>fetch('/api/set_pid?kp='+encodeURIComponent(kp.value)+'&ki='+encodeURIComponent(ki.value)+'&kd='+encodeURIComponent(kd.value)).then(()=>refresh()));
$('save').addEventListener('click',()=>fetch('/api/save_config').then(()=>refresh()));
setInterval(refresh,500);refresh();
</script>
</body>
</html>
)HTML";
