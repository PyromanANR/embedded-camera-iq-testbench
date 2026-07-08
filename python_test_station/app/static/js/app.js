document.addEventListener("DOMContentLoaded", () => {
  const snapshot = document.querySelector("img.snapshot");
  if (snapshot) {
    snapshot.addEventListener("error", () => {
      snapshot.alt = "Snapshot is not available";
    });
  }

  const chart = document.getElementById("powerChart");
  const powerHistory = [];
  if (chart) {
    const initialPower = Number(chart.dataset.initialPower || 0);
    if (initialPower > 0) {
      powerHistory.push(initialPower);
    }
    drawPowerChart(chart, powerHistory);
  }

  if (document.querySelector("[data-telemetry-key]") || chart) {
    setInterval(async () => {
      try {
        const response = await fetch("/api/status", { cache: "no-store" });
        if (!response.ok) {
          return;
        }
        const status = await response.json();
        updateTelemetry(status.telemetry_rows || []);
        updateBattery(status.battery);
        if (chart && status.battery && status.battery.estimated_power_mw) {
          powerHistory.push(Number(status.battery.estimated_power_mw));
          while (powerHistory.length > 40) {
            powerHistory.shift();
          }
          drawPowerChart(chart, powerHistory);
        }
        if (snapshot) {
          snapshot.src = snapshot.src.split("?")[0] + "?ts=" + Date.now();
        }
      } catch (error) {
        // Keep the dashboard readable if the gateway is temporarily offline.
      }
    }, 2500);
  }
});

function updateTelemetry(rows) {
  for (const row of rows) {
    const target = document.querySelector(`[data-telemetry-key="${row.key}"]`);
    if (!target) {
      continue;
    }
    target.textContent = row.unit ? `${row.value} ${row.unit}` : row.value;
  }
}

function updateBattery(battery) {
  if (!battery) {
    return;
  }
  setAll("[data-battery-percent]", battery.percent);
  setAll("[data-battery-runtime]", battery.runtime_label);
  setAll("[data-battery-power]", battery.estimated_power_mw);
  setAll("[data-battery-state]", battery.state);
  setAll("[data-battery-remaining]", battery.remaining_wh);

  const panel = document.querySelector("[data-battery-panel]");
  if (panel) {
    panel.classList.remove("good", "warning", "critical", "offline");
    panel.classList.add(battery.state || "offline");
  }

  for (const module of battery.breakdown || []) {
    setAll(`[data-power-module="${module.key}"]`, `${module.name}: ${module.mw} mW`);
    setAll(`[data-power-module-value="${module.key}"]`, `${module.mw} mW`);
    document.querySelectorAll(`[data-power-module-row="${module.key}"] .module-bar span`).forEach((bar) => {
      bar.style.width = `${module.share}%`;
    });
  }
}

function setAll(selector, value) {
  document.querySelectorAll(selector).forEach((node) => {
    node.textContent = value;
  });
}

function drawPowerChart(canvas, values) {
  const ctx = canvas.getContext("2d");
  const width = canvas.width;
  const height = canvas.height;
  ctx.clearRect(0, 0, width, height);

  ctx.fillStyle = "#f9fbfc";
  ctx.fillRect(0, 0, width, height);

  const padding = 26;
  const plotWidth = width - padding * 2;
  const plotHeight = height - padding * 2;
  const maxValue = Math.max(600, ...values, 1);
  const minValue = 0;

  ctx.strokeStyle = "#d6e0e5";
  ctx.lineWidth = 1;
  for (let i = 0; i <= 4; i++) {
    const y = padding + (plotHeight * i) / 4;
    ctx.beginPath();
    ctx.moveTo(padding, y);
    ctx.lineTo(width - padding, y);
    ctx.stroke();
  }

  ctx.fillStyle = "#63717a";
  ctx.font = "12px Segoe UI, Arial";
  ctx.fillText("Power consumption, mW", padding, 16);
  ctx.fillText(`${Math.round(maxValue)} mW`, width - padding - 64, padding - 8);

  if (values.length < 2) {
    ctx.fillText("Waiting for telemetry samples...", padding, height / 2);
    return;
  }

  ctx.strokeStyle = "#126a77";
  ctx.lineWidth = 3;
  ctx.beginPath();
  values.forEach((value, index) => {
    const x = padding + (plotWidth * index) / Math.max(values.length - 1, 1);
    const y = padding + plotHeight - ((value - minValue) / (maxValue - minValue)) * plotHeight;
    if (index === 0) {
      ctx.moveTo(x, y);
    } else {
      ctx.lineTo(x, y);
    }
  });
  ctx.stroke();

  const last = values[values.length - 1];
  ctx.fillStyle = "#e26d42";
  const x = width - padding;
  const y = padding + plotHeight - ((last - minValue) / (maxValue - minValue)) * plotHeight;
  ctx.beginPath();
  ctx.arc(x, y, 5, 0, Math.PI * 2);
  ctx.fill();
  ctx.fillStyle = "#172026";
  ctx.fillText(`${Math.round(last)} mW`, Math.max(padding, x - 74), Math.max(20, y - 10));
}
