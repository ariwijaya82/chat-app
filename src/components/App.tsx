import React from 'react';

import './app.scss';

import Quicks from "./Quicks"

export default function App() {
  const [state, changeState] = React.useState('close');

  return (
    <Quicks state={state} changeState={changeState} />
  )
}