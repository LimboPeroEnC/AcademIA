/* Frontend del Sistema Academico: mismo origen que el servidor C++ local. */
'use strict';
const api = async (url, config = {}) => {
  const res = await fetch(url, { ...config, headers: { 'Content-Type': 'application/json', ...(config.headers || {}) } });
  const data = await res.json().catch(() => ({}));
  if (!res.ok) throw new Error(data.error || `Error HTTP ${res.status}`);
  return data;
};
const aviso = (container, mensaje, ok = true) => {
  const anterior = container.querySelector('.mensaje-api');
  if (anterior) anterior.remove();
  const p = document.createElement('p');
  p.className = `mensaje-api ${ok ? 'mensaje-ok' : 'mensaje-error'}`;
  p.setAttribute('role', 'status');
  p.textContent = mensaje;
  container.prepend(p);
};
const obtener = (form, nombre) => form.elements.namedItem(nombre)?.value?.trim() || '';
const datos = (form) => Object.fromEntries(new FormData(form).entries());
const nombresCampos = ['rut','nombres','apellido_paterno','apellido_materno','fecha_nacimiento',
  'direccion_domicilio','comuna_residencia','correo_institucional','anio_ingreso','genero','id_carrera','id_usuario'];
async function cargarCarreras() {
  const selects = document.querySelectorAll('select[name="id_carrera"], select[name="carrera"]');
  if (!selects.length) return;
  const lista = await api('/api/carreras');
  selects.forEach(select => {
    const anterior = select.value;
    select.replaceChildren(new Option(select.name === 'carrera' ? 'Todas las carreras' : 'Selecciona una carrera', ''));
    lista.forEach(c => select.add(new Option(c.nombre_carrera, c.id_carrera)));
    select.value = anterior;
  });
}
function crearCelda(fila, contenido) {
  const celda = document.createElement('td');
  celda.textContent = contenido ?? '';
  fila.append(celda);
  return celda;
}

function iniciarRegistro() {
  const form = document.querySelector('form[aria-label="Formulario para registrar estudiantes"]');
  if (!form) return;
  const estado = form.querySelector('[name="estado"]');
  if (estado) { estado.value = 'Activo'; estado.disabled = true; estado.title = 'El registro inicia Activo; el cambio de estado requiere motivo y responsable'; }
  const boton = form.querySelector('.form-actions button'); if (boton) boton.type = 'submit';
  form.addEventListener('submit', async ev => {
    ev.preventDefault();
    if (!form.reportValidity()) return;
    const body = datos(form); body.estado = 'Activo';
    try {
      const r = await api('/api/estudiantes', {method:'POST',body:JSON.stringify(body)});
      aviso(form, `${r.mensaje}. RUT: ${r.rut}.`);
      form.reset();
    } catch (e) { aviso(form, e.message, false); }
  });
}
function iniciarBusqueda() {
  const form = document.querySelector('form[aria-label="Formulario de búsqueda de estudiantes"]');
  if (!form) return;
  const boton = form.querySelector('button[type="button"]'); if (boton) boton.type = 'submit';
  const tabla = document.querySelector('.data-table tbody');
  const texto = document.querySelector('.results-heading p');
  async function buscar(ev) {
    if (ev) ev.preventDefault();
    const params = new URLSearchParams();
    for (const campo of ['rut','nombre','carrera']) {
      const valor = obtener(form,campo); if (valor) params.set(campo,valor);
    }
    try {
      const resultados = await api('/api/estudiantes?' + params.toString());
      tabla.replaceChildren();
      resultados.forEach(e => {
        const fila = document.createElement('tr');
        crearCelda(fila,e.rut);
        const nombre = crearCelda(fila,`${e.nombres} ${e.apellido_paterno} ${e.apellido_materno}`);
        const correo = document.createElement('small'); correo.textContent=e.correo_institucional;nombre.append(correo);
        crearCelda(fila,e.nombre_carrera);
        crearCelda(fila,e.estado);
        const accion = document.createElement('td');const enlace=document.createElement('a');
        enlace.className='table-link';enlace.textContent='Editar';
        enlace.href='modificar.html?rut='+encodeURIComponent(e.rut);
        accion.append(enlace);fila.append(accion);tabla.append(fila);
      });
      if (texto) texto.textContent=`${resultados.length} estudiante(s) encontrado(s); máximo 50 por búsqueda.`;
      if (!resultados.length) aviso(document.querySelector('.results-panel'),'No se encontraron registros.');
    } catch(e) { tabla.replaceChildren(); aviso(document.querySelector('.results-panel'),e.message,false); }
  }
  form.addEventListener('submit',buscar);
  form.addEventListener('reset',()=>setTimeout(buscar,0));
  buscar();
}
function iniciarModificacion() {
  const buscar = document.querySelector('form[aria-label="Buscar estudiante a modificar"]');
  const form = document.querySelector('form[aria-label="Formulario para actualizar estudiantes"]');
  if (!buscar || !form) return;
  const buscarBtn = buscar.querySelector('button[type="button"]');if (buscarBtn) buscarBtn.type='submit';
  const boton=form.querySelector('.form-actions button');if(boton)boton.type='submit';
  const actual = document.getElementById('mod-estado');if(actual)actual.disabled=true;
  let rutCargado='';
  async function cargar(ev) {
    if(ev)ev.preventDefault();
    const rut=obtener(buscar,'rut');
    if(!rut) return;
    try {
      const e=await api('/api/estudiantes/'+encodeURIComponent(rut));
      rutCargado=e.rut;
      nombresCampos.filter(n=>n!=='rut').forEach(c=>{
        const input=form.elements.namedItem(c);if(input) input.value=e[c]||'';
      });
      if(actual)actual.value=e.estado;
      const cambio=document.getElementById('nuevo-estado');if(cambio)cambio.value=e.estado;
      aviso(form,`Datos cargados: ${e.nombres} ${e.apellido_paterno} (${e.rut}).`);
    } catch(e) { rutCargado=''; aviso(form,e.message,false); }
  }
  buscar.addEventListener('submit',cargar);
  form.addEventListener('submit',async ev=>{
    ev.preventDefault();
    if(!rutCargado)return aviso(form,'Primero carga un estudiante mediante su RUT.',false);
    if(!form.reportValidity())return;
    const body=datos(form);body.rut=rutCargado;
    delete body.estado;
    try {
      const r=await api('/api/estudiantes/'+encodeURIComponent(rutCargado),{method:'PUT',body:JSON.stringify(body)});
      aviso(form,r.mensaje);
    } catch(e) { aviso(form,e.message,false); }
  });
  const cambioForm=document.getElementById('form-cambiar-estado');
  if(cambioForm) cambioForm.addEventListener('submit',async ev=>{
    ev.preventDefault();
    if(!rutCargado)return aviso(cambioForm,'Carga un estudiante primero.',false);
    const body=datos(cambioForm);
    try {
      const r=await api('/api/estudiantes/'+encodeURIComponent(rutCargado)+'/estado',{method:'POST',body:JSON.stringify(body)});
      if(actual)actual.value=body.estado;
      aviso(cambioForm,r.mensaje);
    } catch(e) { aviso(cambioForm,e.message,false); }
  });
  const urlRut=new URLSearchParams(location.search).get('rut');
  if(urlRut){ buscar.elements.namedItem('rut').value=urlRut; cargar(); }
}
function iniciarHistorial() {
  const form=document.querySelector('form[aria-label="Filtro de historial del estudiante"]');
  if(!form)return;
  const boton=form.querySelector('button[type="button"]');if(boton)boton.type='submit';
  const panel=document.querySelector('.history-container');
  form.addEventListener('submit',async ev=>{
    ev.preventDefault();
    const rut=obtener(form,'rut');if(!rut)return;
    try {
      const eventos=await api('/api/estudiantes/'+encodeURIComponent(rut)+'/historial');
      const anterior=panel.querySelector('.history-table-live');if(anterior)anterior.remove();
      const vacio=panel.querySelector('.empty-state');if(vacio)vacio.style.display='none';
      const resumen=panel.querySelector('.results-heading p');
      if(resumen)resumen.textContent=`${eventos.length} cambio(s) de estado para el RUT ${rut}.`;
      const table=document.createElement('table');table.className='data-table history-table-live';
      const cab=document.createElement('thead');const f=document.createElement('tr');
      ['Fecha','Estado anterior','Estado nuevo','Motivo','Responsable'].forEach(t=>{
        const c=document.createElement('th');c.textContent=t;f.append(c);
      });cab.append(f);table.append(cab);
      const cuerpo=document.createElement('tbody');
      eventos.forEach(e=>{
        const row=document.createElement('tr');
        [new Date(e.fecha_cambio*1000).toLocaleString('es-CL'),e.estado_anterior,e.estado_nuevo,e.motivo,e.usuario_responsable]
          .forEach(valor=>crearCelda(row,valor));cuerpo.append(row);
      });table.append(cuerpo);panel.append(table);
      if(!eventos.length)aviso(panel,'No hay cambios de estado registrados para este estudiante.');
    } catch(e){aviso(panel,e.message,false);}
  });
}
async function mostrarEstadoBD() {
  const estado=document.querySelector('.environment-note small');
  if(!estado)return;
  try { await api('/api/estado');estado.textContent='Conectado'; }
  catch { estado.textContent='Desconectado'; }
}
document.addEventListener('DOMContentLoaded', async()=>{
  try { await cargarCarreras(); } catch(e) {
    const main=document.querySelector('main');if(main)aviso(main,'No se pudieron cargar las carreras: '+e.message,false);
  }
  iniciarRegistro();iniciarBusqueda();iniciarModificacion();iniciarHistorial();
  mostrarEstadoBD();
});
