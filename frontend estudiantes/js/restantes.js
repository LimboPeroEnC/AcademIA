/* AC-45, AC-50, AC-51, AC-52, AC-54, AC-55, AC-56. API local C++; no credenciales en frontend. */
'use strict';
const $=s=>document.querySelector(s);
async function api(path, options={}) {
  const res=await fetch(path,{...options,headers:{'Content-Type':'application/json',...(options.headers||{})}});
  const data=await res.json().catch(()=>({}));
  if(!res.ok)throw new Error(data.error||`Error HTTP ${res.status}`);
  return data;
}
function aviso(parent,msg,ok=true) {
  const anterior=parent.querySelector('.mensaje-api');if(anterior)anterior.remove();
  const p=document.createElement('p');p.className='mensaje-api'+(ok?'':' mensaje-error');
  p.setAttribute('role','status');p.textContent=msg;parent.prepend(p);
}
function fila(tbody,valores,acciones=[]) {
  const tr=document.createElement('tr');
  valores.forEach(v=>{const td=document.createElement('td');td.textContent=String(v??'');tr.append(td);});
  if(acciones.length){const td=document.createElement('td');acciones.forEach(a=>{const b=document.createElement('button');b.type='button';b.className='table-link';b.textContent=a.nombre;b.style.marginRight='10px';b.addEventListener('click',a.accion);td.append(b);});tr.append(td);}
  tbody.append(tr);
}
function selectOpciones(sel,lista,valor,texto,placeholder='Seleccionar') {
  if(!sel)return;let prev=sel.value;sel.replaceChildren(new Option(placeholder,''));
  lista.forEach(x=>sel.add(new Option(texto(x),valor(x))));
  if([...sel.options].some(o=>o.value===prev))sel.value=prev;
}
function formDatos(form) {return Object.fromEntries(new FormData(form).entries());}
function id(s){return encodeURIComponent(s);}
function vigencia(v){return v?'Sí':'No';}
function errorEn(panel,fn){return (...args)=>{try{return Promise.resolve(fn(...args)).catch(e=>aviso(panel,e.message,false));}catch(e){aviso(panel,e.message,false);}};}

async function facultades(){
  const form=$('#fac-form');if(!form)return;
  const panel=form.closest('section'),idInput=form.elements.namedItem('id_facultad');let editar=false;
  async function recargar(){const datos=await api('/api/facultades');const tabla=$('#fac-lista');tabla.replaceChildren();
    datos.forEach(f=>fila(tabla,[f.id_facultad,f.nombre],[{nombre:'Editar',accion:()=>{editar=true;idInput.value=f.id_facultad;idInput.readOnly=true;form.elements.namedItem('nombre').value=f.nombre;$('#fac-titulo').textContent='Modificar facultad';form.scrollIntoView({behavior:'smooth'});}},{nombre:'Eliminar',accion:errorEn(panel,async()=>{if(!confirm(`¿Eliminar facultad ${f.nombre}? Solo se permite si no tiene carreras.`))return;await api('/api/facultades/'+id(f.id_facultad),{method:'DELETE'});aviso(panel,'Facultad eliminada');await recargar();})}]));}
  $('#fac-nueva').addEventListener('click',()=>{editar=false;form.reset();idInput.readOnly=false;$('#fac-titulo').textContent='Registrar facultad';});
  form.addEventListener('submit',errorEn(panel,async ev=>{ev.preventDefault();const datos=formDatos(form);
    const r=await api(editar?'/api/facultades/'+id(datos.id_facultad):'/api/facultades',{method:editar?'PUT':'POST',body:JSON.stringify(datos)});
    aviso(panel,r.mensaje);await recargar();}));
  await recargar();
}
async function carreras(){
  const form=$('#car-form');if(!form)return;
  const panel=form.closest('section'),idInput=form.elements.namedItem('id_carrera');let editar=false;
  const fac=await api('/api/facultades');selectOpciones($('#car-facultad'),fac,x=>x.id_facultad,x=>x.nombre);
  async function recargar(){const datos=await api('/api/carreras');const tabla=$('#car-lista');tabla.replaceChildren();
    datos.forEach(c=>fila(tabla,[c.id_carrera,c.nombre_carrera,c.nombre_facultad,c.duracion_semestres],[{nombre:'Editar',accion:()=>{
      editar=true;['id_carrera','nombre_carrera','duracion_semestres','modalidad_carrera','id_facultad'].forEach(k=>form.elements.namedItem(k).value=c[k]);idInput.readOnly=true;
      $('#car-titulo').textContent='Modificar carrera';form.scrollIntoView({behavior:'smooth'});}}]));}
  $('#car-nueva').addEventListener('click',()=>{editar=false;form.reset();idInput.readOnly=false;$('#car-titulo').textContent='Registrar carrera';});
  form.addEventListener('submit',errorEn(panel,async ev=>{ev.preventDefault();const d=formDatos(form);d.duracion_semestres=Number(d.duracion_semestres);
    const r=await api(editar?'/api/carreras/'+id(d.id_carrera):'/api/carreras',{method:editar?'PUT':'POST',body:JSON.stringify(d)});
    aviso(panel,r.mensaje);await recargar();}));
  await recargar();
}
async function planes(){
  const form=$('#pla-form');if(!form)return;const panel=form.closest('section');let editar=false;
  const idInput=form.elements.namedItem('id_plan'),carrera=form.elements.namedItem('id_carrera');
  const cs=await api('/api/carreras');selectOpciones(carrera,cs,x=>x.id_carrera,x=>x.nombre_carrera);
  async function recargar(){const lista=await api('/api/planes');const t=$('#pla-lista');t.replaceChildren();
    lista.forEach(p=>fila(t,[p.id_plan,p.nombre_plan,cs.find(c=>c.id_carrera===p.id_carrera)?.nombre_carrera||p.id_carrera,p.anio_plan,vigencia(p.vigente)],[{nombre:'Modificar',accion:()=>{
      editar=true;['id_plan','id_carrera','nombre_plan','anio_plan'].forEach(k=>form.elements.namedItem(k).value=p[k]);form.elements.namedItem('vigente').checked=p.vigente;
      idInput.readOnly=true;carrera.disabled=true;$('#pla-titulo').textContent='Actualizar plan';form.scrollIntoView({behavior:'smooth'});}}]));}
  $('#pla-nuevo').addEventListener('click',()=>{editar=false;form.reset();idInput.readOnly=false;carrera.disabled=false;$('#pla-titulo').textContent='Crear plan';});
  form.addEventListener('submit',errorEn(panel,async ev=>{ev.preventDefault();const d=formDatos(form);d.id_carrera=carrera.value;d.anio_plan=Number(d.anio_plan);d.vigente=form.elements.namedItem('vigente').checked;
    const r=await api(editar?'/api/planes/'+id(d.id_plan):'/api/planes',{method:editar?'PUT':'POST',body:JSON.stringify(d)});
    aviso(panel,r.mensaje);await recargar();}));
  await recargar();
}
async function asociarAsignaturas(){
  const form=$('#aa-form');if(!form)return;const panel=form.closest('section');
  const planes=await api('/api/planes');selectOpciones($('#aa-plan'),planes,x=>x.id_plan,x=>`${x.nombre_plan} (${x.id_carrera})`);
  const ramos=await api('/api/asignaturas');selectOpciones($('#aa-ramo'),ramos,x=>x.id_ramo,x=>`${x.id_ramo} · ${x.nombre_ramo}`);
  async function recargar(){const plan=$('#aa-plan').value,t=$('#aa-lista');t.replaceChildren();if(!plan)return;
    const lista=await api('/api/planes/'+id(plan)+'/asignaturas');
    lista.forEach(r=>fila(t,[r.id_ramo,r.nombre_ramo,r.semestre,r.cantidad_creditos],[{nombre:'Cambiar semestre',accion:()=>{
      form.elements.namedItem('id_ramo').value=r.id_ramo;form.elements.namedItem('semestre').value=r.semestre;form.scrollIntoView({behavior:'smooth'});
    }},{nombre:'Quitar',accion:errorEn(panel,async()=>{if(!confirm(`¿Quitar ${r.nombre_ramo} del plan? Esto también elimina sus vínculos de prerrequisitos.`))return;
      await api('/api/planes/'+id(plan)+'/asignaturas/'+id(r.id_ramo),{method:'DELETE'});aviso(panel,'Asignatura retirada');await recargar();})}]));}
  $('#aa-cargar').addEventListener('click',errorEn(panel,recargar));$('#aa-plan').addEventListener('change',errorEn(panel,recargar));
  form.addEventListener('submit',errorEn(panel,async ev=>{ev.preventDefault();if(!$('#aa-plan').value)throw new Error('Selecciona un plan');
    const d=formDatos(form);d.semestre=Number(d.semestre);
    const r=await api('/api/planes/'+id($('#aa-plan').value)+'/asignaturas',{method:'POST',body:JSON.stringify(d)});
    aviso(panel,r.mensaje);await recargar();}));
}
async function prerrequisitos(){
  const form=$('#pre-form');if(!form)return;const panel=form.closest('section');
  const planes=await api('/api/planes');selectOpciones($('#pre-plan'),planes,x=>x.id_plan,x=>`${x.nombre_plan} (${x.id_carrera})`);
  async function recargar(){const plan=$('#pre-plan').value,t=$('#pre-lista');t.replaceChildren();if(!plan)return;
    const lista=await api('/api/planes/'+id(plan)+'/asignaturas');
    selectOpciones($('#pre-ramo'),lista,x=>x.id_ramo,x=>`${x.nombre_ramo} · Sem ${x.semestre}`);
    selectOpciones($('#pre-requisito'),lista,x=>x.id_ramo,x=>`${x.nombre_ramo} · Sem ${x.semestre}`);
    const requisitos=await api('/api/planes/'+id(plan)+'/prerrequisitos');
    requisitos.forEach(r=>fila(t,[r.nombre_ramo+' ('+r.id_ramo+')',r.nombre_prerrequisito+' ('+r.id_prerrequisito+')'],[{nombre:'Quitar',accion:errorEn(panel,async()=>{
      if(!confirm('¿Eliminar esta dependencia?'))return;
      await api('/api/planes/'+id(plan)+'/prerrequisitos/'+id(r.id_ramo)+'/'+id(r.id_prerrequisito),{method:'DELETE'});
      aviso(panel,'Prerrequisito eliminado');await recargar();})}]));}
  $('#pre-cargar').addEventListener('click',errorEn(panel,recargar));$('#pre-plan').addEventListener('change',errorEn(panel,recargar));
  form.addEventListener('submit',errorEn(panel,async ev=>{ev.preventDefault();const plan=$('#pre-plan').value;if(!plan)throw new Error('Selecciona un plan');
    const d=formDatos(form);const r=await api('/api/planes/'+id(plan)+'/prerrequisitos',{method:'POST',body:JSON.stringify(d)});
    aviso(panel,r.mensaje);await recargar();}));
}
async function creditos(){
  const form=$('#cre-form');if(!form)return;const panel=form.closest('section');
  async function recargar(){const q=$('#cre-filtro').value.trim(),ramos=await api('/api/asignaturas?q='+id(q));const t=$('#cre-lista');t.replaceChildren();
    ramos.forEach(r=>fila(t,[r.id_ramo,r.nombre_ramo,r.cantidad_creditos],[{nombre:'Editar',accion:()=>{$('#cre-id').value=r.id_ramo;$('#cre-valor').value=r.cantidad_creditos;form.scrollIntoView({behavior:'smooth'});}}]));}
  $('#cre-buscar').addEventListener('click',errorEn(panel,recargar));
  form.addEventListener('submit',errorEn(panel,async ev=>{ev.preventDefault();if(!$('#cre-id').value)throw new Error('Selecciona una asignatura');
    const r=await api('/api/asignaturas/'+id($('#cre-id').value)+'/creditos',{method:'PATCH',body:JSON.stringify({cantidad_creditos:Number($('#cre-valor').value)})});
    aviso(panel,r.mensaje);await recargar();}));await recargar();
}
async function ficha(){
  const form=$('#fic-form');if(!form)return;const panel=form.closest('section');
  form.addEventListener('submit',errorEn(panel,async ev=>{ev.preventDefault();const rut=$('#fic-rut').value.trim();
    const d=await api('/api/estudiantes/'+id(rut)+'/ficha');$('#fic-resultado').hidden=false;
    const datos=$('#fic-datos');datos.replaceChildren();
    [['RUT',d.rut],['Estudiante',[d.nombres,d.apellido_paterno,d.apellido_materno].join(' ')],['Correo',d.correo_institucional],['Estado',d.estado],['Carrera',d.nombre_carrera],['Facultad',d.nombre_facultad],['Plan',d.nombre_plan||'Sin plan asociado'],['Ingreso',d.anio_ingreso]].forEach(([titulo,valor])=>{
      const div=document.createElement('div'),dt=document.createElement('dt'),dd=document.createElement('dd');dt.textContent=titulo;dd.textContent=valor;div.append(dt,dd);datos.append(div);});
    const tbody=$('#fic-ramos');tbody.replaceChildren();d.asignaturas_plan.forEach(x=>fila(tbody,[x.semestre,x.nombre,x.cantidad_creditos]));
    if(!d.asignaturas_plan.length)fila(tbody,['—','Sin asignaturas del plan','—']);
  }));
}
document.addEventListener('DOMContentLoaded',()=>{
  const screen=$('.main-content');
  (async()=>{
    await api('/api/estado');
    await Promise.all([facultades(),carreras(),planes(),asociarAsignaturas(),prerrequisitos(),creditos(),ficha()]);
  })().catch(e=>{if(screen)aviso(screen,'No se pudo inicializar el módulo: '+e.message,false);});
});
