/* AC-47 / AC-48 / AC-53 / AC-57. Consumir la API C++ del mismo origen. */
'use strict';
const $ = s => document.querySelector(s);
const api = async (path, options={}) => {
  const res=await fetch(path,{...options,headers:{'Content-Type':'application/json',...(options.headers||{})}});
  const body=await res.json().catch(()=>({}));
  if(!res.ok)throw new Error(body.error || `Error HTTP ${res.status}`);
  return body;
};
function mensaje(contenedor, texto, exito=true) {
  const previo=contenedor.querySelector('.mensaje-api');if(previo)previo.remove();
  const el=document.createElement('p');el.className='mensaje-api'+(exito?'':' mensaje-error');
  el.setAttribute('role','status');el.textContent=texto;contenedor.prepend(el);
}
function celda(fila,dato) {const e=document.createElement('td');e.textContent=String(dato??'');fila.append(e);return e;}
function opciones(select,lista,valor,label,placeholder) {
  const actual=select.value;select.replaceChildren(new Option(placeholder,''));
  lista.forEach(e=>select.add(new Option(label(e),valor(e))));
  if([...select.options].some(o=>o.value===actual)) select.value=actual;
}
function normalizarRut(s) {return s.trim().toUpperCase();}
async function cargarCarreras() {
  const carreras=await api('/api/carreras');
  ['#malla-carrera','#plan-carrera','#asoc-carrera'].forEach(s=>{
    const el=$(s);if(el)opciones(el,carreras,c=>c.id_carrera,c=>c.nombre_carrera,'Seleccionar carrera');
  });
}
function iniciarAsignaturas() {
  const buscar=$('#buscar-ramo'),form=$('#form-ramo'),tabla=$('#tabla-ramos');if(!form)return;
  let editando=false;
  async function recargar(ev) {
    if(ev)ev.preventDefault();
    try {
      const lista=await api('/api/asignaturas?q='+encodeURIComponent($('#filtro-ramo').value.trim()));
      tabla.replaceChildren();
      lista.forEach(a=>{
        const fila=document.createElement('tr');
        celda(fila,a.id_ramo);celda(fila,a.nombre_ramo);celda(fila,a.cantidad_creditos);celda(fila,a.numero_semestre);
        const accion=document.createElement('td'),btn=document.createElement('button');
        btn.type='button';btn.className='table-link';btn.textContent='Modificar';
        btn.addEventListener('click',()=>{
          for(const n of ['id_ramo','nombre_ramo','descripcion','cantidad_creditos','numero_semestre'])form.elements.namedItem(n).value=a[n];
          form.elements.namedItem('id_ramo').readOnly=true;editando=true;
          $('#titulo-editor-ramo').textContent='Modificar asignatura';$('#guardar-ramo').textContent='Guardar cambios';
          form.scrollIntoView({behavior:'smooth'});
        });accion.append(btn);fila.append(accion);tabla.append(fila);
      });
      if(!lista.length)mensaje(buscar,'No se encontraron asignaturas.');
    }catch(e){mensaje(buscar,e.message,false);}
  }
  buscar.addEventListener('submit',recargar);
  $('#nuevo-ramo').addEventListener('click',()=>{
    editando=false;form.reset();form.elements.namedItem('id_ramo').readOnly=false;
    $('#titulo-editor-ramo').textContent='Registrar asignatura';$('#guardar-ramo').textContent='Registrar asignatura';
  });
  form.addEventListener('submit',async ev=>{
    ev.preventDefault();if(!form.reportValidity())return;
    const data=Object.fromEntries(new FormData(form));
    data.cantidad_creditos=Number(data.cantidad_creditos);data.numero_semestre=Number(data.numero_semestre);
    try {
      const id=encodeURIComponent(data.id_ramo);
      const r=await api(editando?'/api/asignaturas/'+id:'/api/asignaturas',{
        method:editando?'PUT':'POST',body:JSON.stringify(data)});
      mensaje(form,r.mensaje);await recargar();
    }catch(e){mensaje(form,e.message,false);}
  });
  recargar();
}
function iniciarMalla() {
  const selector=$('#malla-carrera');if(!selector)return;
  const selectorPlan=$('#malla-plan'),grid=$('#malla-cuadricula');
  async function cargarPlanes() {
    grid.replaceChildren();
    try {
      const planes=await api('/api/planes?carrera='+encodeURIComponent(selector.value));
      opciones(selectorPlan,planes,p=>p.id_plan,p=>`${p.nombre_plan} (${p.anio_plan})`,'Seleccionar plan');
    }catch(e){mensaje($('#panel-malla'),e.message,false);}
  }
  selector.addEventListener('change',cargarPlanes);
  $('#ver-malla').addEventListener('click',async()=>{
    const id=selectorPlan.value;if(!id)return mensaje($('#panel-malla'),'Selecciona un plan.',false);
    try {
      const lista=await api('/api/planes/'+encodeURIComponent(id)+'/malla');
      grid.replaceChildren();
      const semestres=new Map();let creditos=0;
      lista.forEach(r=>{if(!semestres.has(r.semestre))semestres.set(r.semestre,[]);semestres.get(r.semestre).push(r);creditos+=r.cantidad_creditos;});
      [...semestres].sort((a,b)=>a[0]-b[0]).forEach(([semestre,ramos])=>{
        const div=document.createElement('article');div.className='semester';
        const h=document.createElement('h3');h.textContent=`Semestre ${semestre}`;div.append(h);
        ramos.forEach(r=>{const item=document.createElement('div');item.className='malla-ramo';
          const nombre=document.createElement('strong');nombre.textContent=r.nombre_ramo;
          const met=document.createElement('small');met.textContent=`${r.id_ramo} · ${r.cantidad_creditos} créditos`;
          item.append(nombre,met);div.append(item);});grid.append(div);
      });
      $('#malla-resumen').textContent=`${lista.length} asignaturas · ${creditos} créditos · ${semestres.size} semestres con ramos.`;
      if(!lista.length)mensaje($('#panel-malla'),'Este plan aún no tiene ramos asociados. AC-55 está pendiente; consulta sql/demo_malla_OPCIONAL.sql para probar la malla.');
    }catch(e){mensaje($('#panel-malla'),e.message,false);}
  });
  $('#form-plan').addEventListener('submit',async ev=>{
    ev.preventDefault();const form=ev.currentTarget;if(!form.reportValidity())return;
    const data=Object.fromEntries(new FormData(form));data.anio_plan=Number(data.anio_plan);
    try {const r=await api('/api/planes',{method:'POST',body:JSON.stringify(data)});
      mensaje($('#creador-plan'),r.mensaje);form.reset();selector.value=data.id_carrera;await cargarPlanes();selectorPlan.value=data.id_plan;
    }catch(e){mensaje($('#creador-plan'),e.message,false);}
  });
}
function iniciarAsociarPlan() {
  const buscar=$('#buscar-estudiante-plan');if(!buscar)return;
  const carrera=$('#asoc-carrera'),selector=$('#asoc-plan');let rutCargado='';
  async function cargarPlanes(seleccionado='') {
    try{const planes=await api('/api/planes?carrera='+encodeURIComponent(carrera.value));
      opciones(selector,planes.filter(p=>p.vigente),p=>p.id_plan,p=>`${p.nombre_plan} (${p.anio_plan})`,'Seleccionar plan vigente');
      if(seleccionado)selector.value=seleccionado;
    }catch(e){mensaje($('#form-asociar'),e.message,false);}
  }
  carrera.addEventListener('change',()=>cargarPlanes());
  buscar.addEventListener('submit',async ev=>{
    ev.preventDefault();const rut=normalizarRut($('#plan-rut').value);
    rutCargado='';$('#plan-estudiante-info').textContent='Cargando...';$('#plan-asignado-info').textContent='';
    try {
      const e=await api('/api/estudiantes/'+encodeURIComponent(rut));rutCargado=e.rut;
      $('#plan-estudiante-info').textContent=`${e.nombres} ${e.apellido_paterno} ${e.apellido_materno} · Carrera actual: ${e.nombre_carrera} · ${e.estado}`;
      carrera.value=e.id_carrera;await cargarPlanes();
      try {const a=await api('/api/estudiantes/'+encodeURIComponent(rut)+'/plan');
        $('#plan-asignado-info').textContent=`Plan asignado: ${a.nombre_plan} (${a.id_plan})`;
        if(carrera.value===a.id_carrera)selector.value=a.id_plan;
      }catch(e){if(!e.message.includes('sin plan'))throw e;$('#plan-asignado-info').textContent='Sin plan de estudios asignado.';}
    }catch(e){mensaje(buscar,e.message,false);$('#plan-estudiante-info').textContent='Sin estudiante seleccionado.';}
  });
  $('#form-asociar').addEventListener('submit',async ev=>{
    ev.preventDefault();if(!rutCargado)return mensaje($('#form-asociar'),'Primero consulta un estudiante por RUT.',false);
    const data={id_carrera:carrera.value,id_plan:selector.value};if(!data.id_carrera||!data.id_plan)return;
    try {const r=await api('/api/estudiantes/'+encodeURIComponent(rutCargado)+'/plan',{
      method:'PUT',body:JSON.stringify(data)});
      mensaje($('#form-asociar'),r.mensaje);
      const actual=await api('/api/estudiantes/'+encodeURIComponent(rutCargado)+'/plan');
      $('#plan-asignado-info').textContent=`Plan asignado: ${actual.nombre_plan} (${actual.id_plan})`;
    }catch(e){mensaje($('#form-asociar'),e.message,false);}
  });
}
function iniciarEstado() {
  const buscar=$('#buscar-estado');if(!buscar)return;
  let rutCargado='';let estadoActual='';
  async function historial() {
    const lista=await api('/api/estudiantes/'+encodeURIComponent(rutCargado)+'/historial');
    const tabla=$('#tabla-estado-historial');tabla.replaceChildren();
    lista.forEach(h=>{const fila=document.createElement('tr');
      [new Date(h.fecha_cambio*1000).toLocaleString('es-CL'),h.estado_anterior,h.estado_nuevo,h.motivo,h.usuario_responsable]
        .forEach(v=>celda(fila,v));tabla.append(fila);});
    if(!lista.length){const fila=document.createElement('tr');const td=celda(fila,'Sin cambios registrados');td.colSpan=5;tabla.append(fila);}
  }
  buscar.addEventListener('submit',async ev=>{
    ev.preventDefault();rutCargado='';const rut=normalizarRut($('#estado-rut').value);
    try {const e=await api('/api/estudiantes/'+encodeURIComponent(rut));rutCargado=e.rut;estadoActual=e.estado;
      $('#estado-resumen').textContent=`${e.nombres} ${e.apellido_paterno} · Estado actual: ${e.estado}`;
      $('#estado-nuevo').value=e.estado==='Activo'?'Inactivo':'Activo';await historial();
    }catch(e){mensaje(buscar,e.message,false);$('#estado-resumen').textContent='No se pudo cargar el estudiante.';}
  });
  $('#form-estado').addEventListener('submit',async ev=>{
    ev.preventDefault();if(!rutCargado)return mensaje($('#form-estado'),'Primero carga el estudiante.',false);
    const form=ev.currentTarget;if(!form.reportValidity())return;
    const datos=Object.fromEntries(new FormData(form));
    if(datos.estado===estadoActual)return mensaje(form,'El estudiante ya tiene ese estado.',false);
    try {const r=await api('/api/estudiantes/'+encodeURIComponent(rutCargado)+'/estado',{
      method:'POST',body:JSON.stringify(datos)});
      estadoActual=datos.estado;$('#estado-resumen').textContent=`Estado actual: ${datos.estado}`;
      mensaje(form,r.mensaje);await historial();
    }catch(e){mensaje(form,e.message,false);}
  });
}
document.addEventListener('DOMContentLoaded',async()=>{
  try {await cargarCarreras();}catch(e){const panel=$('.main-content');if(panel)mensaje(panel,'Error cargando carreras: '+e.message,false);}
  iniciarAsignaturas();iniciarMalla();iniciarAsociarPlan();iniciarEstado();
  const estado=$('.environment-note small');if(estado){try{await api('/api/estado');estado.textContent='Conectado';}catch{estado.textContent='Desconectado';}}
});
