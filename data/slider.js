function updateJoystickPosition(state) {
  state.element.style.setProperty('--jd', (state.pos * 120) + 'px');
  state.callback(state.pos);
}

function handleJoystickPosition(state, X) {
  const rect = state.element.getBoundingClientRect();
  const x = X - rect.left;
  const d = x - rect.width / 2;
  const maxD = rect.width / 2 - 25;

  const value = d / maxD;
  const clampedValue = Math.min(Math.max(value, -1), 1);

  state.pos = clampedValue;
  updateJoystickPosition(state);
}

function sliderFactory(element, callback) {
  const state = {
    pos: 0,
    element,
    callback,
    moving: false
  };

  element.addEventListener('touchstart', (e) => {
    state.moving = true;
    element.classList.add('moving');
    handleJoystickPosition(state, e.touches[0].clientX);
  });
  element.addEventListener('mousedown', (e) => {
    e.preventDefault();
    state.moving = true;
    element.classList.add('moving');
    handleJoystickPosition(state, e.clientX);
  });

  window.addEventListener('touchmove', (e) => {
    if (!state.moving) return;

    handleJoystickPosition(state, e.touches[0].clientX);
  });

  window.addEventListener('mousemove', (e) => {
    e.preventDefault();
    if (!state.moving) return;

    handleJoystickPosition(state, e.clientX);
  });

  window.addEventListener('touchend', (e) => {
    if (!state.moving) return;

    state.moving = false;
    state.pos = 0;
    element.classList.remove('moving');

    element.style.setProperty('--jd', state.pos + 'px');
    callback(state.pos);
  });

  window.addEventListener('mouseup', (e) => {
    e.preventDefault();
    if (!state.moving) return;

    state.moving = false;
    state.pos = 0;
    element.classList.remove('moving');

    element.style.setProperty('--jd', state.pos + 'px');
    callback(state.pos);
  });
}