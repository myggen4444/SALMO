const ctx = document.getElementById("dataChart").getContext("2d");

const maxPoints = 30;

let tempHist = [];
let pressHist = [];
let salHist = [];

const rect = ctx.canvas.getBoundingClientRect();
ctx.canvas.width = rect.width;
ctx.canvas.height = rect.height;

// ----------- HENT LIVE DATA -----------
async function fetchLive() {
    const res = await fetch("/data");
    return await res.json();
}

// ----------- HENT HISTORIKK -----------
async function fetchHistory() {
    const res = await fetch("/history");
    const data = await res.json();

    tempHist = data.temp.filter(v => v !== null);
    pressHist = data.pressure.filter(v => v !== null);
    salHist = data.salinity.filter(v => v !== null);

    draw();
}

// ----------- GRAF -----------
function draw() {
    const c = ctx.canvas;
    ctx.clearRect(0, 0, c.width, c.height);

    const margin = { left: 50, right: 20, top: 20, bottom: 40 };
    const width = c.width - margin.left - margin.right;
    const height = c.height - margin.top - margin.bottom;

    // ----------- BAKGRUNN -----------
    ctx.fillStyle = "#fff";
    ctx.fillRect(0, 0, c.width, c.height);

    // ----------- DATA -----------
    const allData = [...tempHist, ...pressHist, ...salHist];
    if (!allData.length) return;

    const minVal = Math.min(...allData);
    const maxVal = Math.max(...allData);
    const range = (maxVal - minVal) || 1;

    const maxLen = Math.max(
        tempHist.length,
        pressHist.length,
        salHist.length
    );

    const stepX = maxLen > 1 ? width / (maxLen - 1) : 0;

    // ----------- RUTENETT -----------
    ctx.strokeStyle = "#eee";
    ctx.lineWidth = 1;

    const gridLines = 5;

    for (let i = 0; i <= gridLines; i++) {
        const y = margin.top + (i / gridLines) * height;
        ctx.beginPath();
        ctx.moveTo(margin.left, y);
        ctx.lineTo(margin.left + width, y);
        ctx.stroke();
    }

    for (let i = 0; i <= gridLines; i++) {
        const x = margin.left + (i / gridLines) * width;
        ctx.beginPath();
        ctx.moveTo(x, margin.top);
        ctx.lineTo(x, margin.top + height);
        ctx.stroke();
    }

    // ----------- AKSER -----------
    ctx.strokeStyle = "#333";
    ctx.lineWidth = 2;

    ctx.beginPath();
    ctx.moveTo(margin.left, margin.top);
    ctx.lineTo(margin.left, margin.top + height);
    ctx.lineTo(margin.left + width, margin.top + height);
    ctx.stroke();

    // ----------- TEKST -----------
    ctx.fillStyle = "#000";
    ctx.font = "14px Arial";

    ctx.textAlign = "center";
    ctx.textBaseline = "top";
    ctx.fillText("Tid", margin.left + width / 2, margin.top + height + 10);

    ctx.save();
    ctx.translate(15, margin.top + height / 2);
    ctx.rotate(-Math.PI / 2);
    ctx.textAlign = "center";
    ctx.fillText("Verdi", 0, 0);
    ctx.restore();

    // ----------- LINJER -----------
    const drawLine = (data, color) => {
        if (data.length < 2) return;

        ctx.strokeStyle = color;
        ctx.lineWidth = 2;
        ctx.lineJoin = "round";
        ctx.lineCap = "round";

        ctx.beginPath();

        data.forEach((v, i) => {
            const x = margin.left + i * stepX;
            const y = margin.top + height - ((v - minVal) / range) * height;

            if (i === 0) ctx.moveTo(x, y);
            else ctx.lineTo(x, y);
        });

        ctx.stroke();
    };

    drawLine(tempHist, "#e74c3c");
    drawLine(pressHist, "#3498db");
    drawLine(salHist, "#2ecc71");

    // ----------- LEGENDE -----------
    const legend = [
        { text: "Temperatur", color: "#e74c3c" },
        { text: "Trykk", color: "#3498db" },
        { text: "Salinitet", color: "#2ecc71" }
    ];

    const legendX = c.width - margin.right - 120;
    const legendY = margin.top + 10;

    legend.forEach((item, i) => {
        const y = legendY + i * 18;

        ctx.fillStyle = item.color;
        ctx.fillRect(legendX, y - 8, 10, 10);

        ctx.fillStyle = "#000";
        ctx.textAlign = "left";
        ctx.textBaseline = "middle";
        ctx.fillText(item.text, legendX + 15, y - 3);
    });
}

// ----------- LIVE UPDATE -----------
async function updateLive() {
    try {
        const d = await fetchLive();

        document.getElementById("temperature").textContent =
            d.temp !== null ? d.temp : "–";

        document.getElementById("pressure").textContent =
            d.pressure !== null ? d.pressure : "–";

        document.getElementById("salinity").textContent =
            d.salinity !== null ? d.salinity : "–";

        // temp
        if (d.temp !== null) {
            tempHist.push(d.temp);
            if (tempHist.length > maxPoints) tempHist.shift();
        }

        // pressure
        if (d.pressure !== null) {
            pressHist.push(d.pressure);
            if (pressHist.length > maxPoints) pressHist.shift();
        }

        // salinity
        if (d.salinity !== null) {
            salHist.push(d.salinity);
            if (salHist.length > maxPoints) salHist.shift();
        }

        draw();

        document.getElementsByClassName("dot online")[0].style.backgroundColor = "#2ecc71";
        document.getElementById("status_text").textContent = "ONLINE";

    } catch (e) {
        document.getElementsByClassName("dot online")[0].style.backgroundColor = "#c71f1f";
        document.getElementById("status_text").textContent = "OFFLINE";
    }
}

// ----------- INIT -----------
async function init() {
    await fetchHistory();
    updateLive();
    setInterval(updateLive, 2000);
}

init();