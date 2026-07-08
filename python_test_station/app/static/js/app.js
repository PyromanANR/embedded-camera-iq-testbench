document.addEventListener("DOMContentLoaded", () => {
  const snapshot = document.querySelector("img.snapshot");
  if (!snapshot) {
    return;
  }
  snapshot.addEventListener("error", () => {
    snapshot.alt = "Snapshot is not available";
  });
});

