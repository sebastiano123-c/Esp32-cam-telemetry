

document.addEventListener("DOMContentLoaded", function(){

    // batteryLevel = document.getElementById("battery");

    // flightMode = document.getElementById("flightMode");

    document.getElementById("PID-toggle-menu").onclick = function(){menuShowHide(this, "menu-pid")};

    document.getElementById("cam-toggle-menu").onclick = function(){menuShowHide(this, "menu-cam")};

    document.getElementById("telem-toggle-menu").onclick = function(){menuShowHide(this, "left-panel")};


    // ON TIMEOUT
    // const levelTimeout = setTimeout(flightModeString(flightMode), 100);
    // setTimeout(batteryLevelColor(document.getElementById("battery")), 100);

    // var batt = 12.6;
    // setInterval(function(){
    //     var bbb = document.getElementById("battery");
    //     bbb.innerHTML = batt;
    //     batteryLevelColor(bbb);
    //     batt -= 1;
    //     if (batt <= 1) batt = 12.6;
    // }, 500)

    // functions
    function flightModeString(e){
        if(e.innerHTML == 1)      e.innerHTML = "auto-leveling";
        else if(e.innerHTML == 2) e.innerHTML = "altitude hold";
        else if(e.innerHTML == 3) e.innerHTML = "altitude + GPS";
    };


    function batteryLevelColor(e){
        var batteryLevelInnerHTML = e.innerHTML;
        var batteryLevelFloat = parseFloat(batteryLevelInnerHTML);
        var width = parseFloat(e.parentElement.parentElement.clientWidth);
        var percent = batteryLevelFloat / 12.6;  // 12.6 V - 10 V

    // console.log(batteryLevelFloat, percent);

        e.parentElement.style.background = "green";
        e.parentElement.style.width = width * percent +"px";
        alertMessage(0)
        // alert( width );
        if(batteryLevelFloat <= 11.3){
            e.parentElement.style.background = "orange";
            alertMessage("Battery low!", "orange")
        }
        if(batteryLevelFloat <= 10.5){
            e.parentElement.style.background = "red";
            alertMessage("DANGER battery level!", "red");
        }
    };

    function menuShowHide(thisElm, e){
        var elm = document.getElementById(e);
        if(elm.className.includes(" hidden")){
            elm.className = elm.className.split(" hidden")[0];
            thisElm.style.opacity = .7;
        }
        else{
            elm.className = elm.className + " hidden";
            thisElm.style.opacity = 1.;
        }
        
    }

    function updateSDStatus(e){
        var parE = e.parentElement;
        var stat = e.innerHTML;
        if(stat==1) e.innerHTML = "conn.";
        else if(stat==0) e.innerHTML = "disc.";
    }

    var ef=document.getElementById("alert-msg");

    function alertMessage(msg, color="orange"){
        if(msg!=0){
            ef.classList.remove("hidden");
            ef.style.background=color;
            ef.innerHTML = msg;
        }else ef.classList.add("hidden");
    }


    //      (TELEMETRY)
    setInterval(ee,100);function ee(){
        var ee = `${document.location.origin}/telemetry`;
        fetch(ee).then(
            res=>{return res.json()}
        ).then(
            data => {for (var key in data) {
                document.getElementById(key).innerHTML=data[key];
            };
            updateSDStatus(document.getElementById("SDStatus"));
            flightModeString(document.getElementById("flightMode"));
            batteryLevelColor(document.getElementById("battery"));
        });
    }



    //      (CONTROL CAM)
    function c(e){
        let t;
        switch(e.type){
            case"checkbox":t=e.checked?1:0;break;
            case"range":case"select-one":t=e.value;break;
            case"button":case"submit":t="1";break;default:return
        }
        const o=`${n}/control?var=${e.id}&val=${t}`;
        fetch(o)/*.then(e=>{
            console.log(`request to ${o} finished, status: ${e.status}`)
        })*/
    }

    
    //      (PID)
    var n=document.location.origin;
    document.querySelectorAll("input[type=text].default-action").forEach(
        e=>{
            e.onkeyup=()=>{
                !function(e){
                    const t=`${n}/pid?var=${e.id}&val=${e.value}`;
                    fetch(t)/*.then(e=>{
                        console.log(`request to ${t}finished,status:${e.status}`)
                    })*/
                }(e)
            }
        }
    );


    //      
    const a=e=>{
        e.classList.add("hidden")
    },u=e=>{
        e.classList.remove("hidden")
    },d=e=>{
        e.classList.add("disabled"),e.disabled=!0
    },s=e=>{
        e.classList.remove("disabled"),e.disabled=!1
    },o=(e,t,o)=>{
        let n;
        o=!(null!=o)||o,"checkbox"===e.type?(n=e.checked,t=!!t,e.checked=t):(n=e.value,e.value=t),o&&n!==t?c(e):!o&&("aec"===e.id?t?a(v):u(v):"agc"===e.id?t?(u($),a(b)):(a($),u(b)):"awb_gain"===e.id?t?u(_):a(_):"face_recognize"===e.id&&(t?s(l):d(l)))
    };
    document.querySelectorAll(".close").forEach(
        e=>{
            e.onclick=()=>{
             a(e.parentNode)
            }
        }),fetch(`${n}/status`).then(function(e){
            return e.json()
        }).then(function(t){
            document.querySelectorAll(".default-action").forEach(e=>{o(e,t[e.id],!1)})
        });


    
    const e=document.getElementById("stream"),
    i=document.getElementById("get-still"),
    r=document.getElementById("toggle-stream"),
    l=document.getElementById("face_enroll");
    // f=()=>{
    //     window.stop(),
    //     r.innerHTML="Start Stream"
    // };
    // //      (CAPTURE)
    // i.onclick=()=>{
    //     e.src=`${n}/capture?_cb=${Date.now()}`
    // },
    //      (STREAMING)
    e.src=`${n+":81"}/stream`,
    l.onclick=()=>{
        c(l)
    },document.querySelectorAll(".default-action").forEach(e=>{
        e.onchange=()=>c(e)
    });


    const g=document.getElementById("agc"),
    b=document.getElementById("agc_gain-group"),
    $=document.getElementById("gainceiling-group");

    g.onchange=()=>{
        c(g),
        g.checked?(
            u($),
            a(b)
        ):(
            a($),
            u(b)
        )
    };


    const h=document.getElementById("aec"),
    v=document.getElementById("aec_value-group");


    h.onchange=()=>{
        c(h),
        h.checked?a(v):u(v)
    };


    const p=document.getElementById("awb_gain"),
    _=document.getElementById("wb_mode-group");


    p.onchange=()=>{c(p),p.checked?u(_):a(_)};


    const w=document.getElementById("face_detect"),
    S=document.getElementById("face_recognize"),
    k=document.getElementById("framesize");


    k.onchange=()=>{
        c(k),
        5<k.value&&(o(w,!1),o(S,!1))
    },w.onchange=()=>5<k.value?(
        alert("Please lower resolution"),
        void o(w,!1)
    ):(
        c(w),
        void(!w.checked&&(d(l),o(S,!1)))
    ),S.onchange=()=>5<k.value?(
        alert("Please lower resolution"),void o(S,!1)
    ):(
        c(S),
        void(S.checked?(s(l),o(w,!0)):d(l))
    );

});
