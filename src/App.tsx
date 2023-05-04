import React from 'react';

import StartPage from './components/StartPage';
import Layout from './components/Layout';

import './style/costum.scss';

export default function App() {
  const [slide, setSlide] = React.useState(false);

  React.useEffect(() => {
    setTimeout(() => setSlide(true), 500);
  });

  return (
    <div className='app'>
      <StartPage slide={slide} />
      <Layout />
    </div>
  )
}