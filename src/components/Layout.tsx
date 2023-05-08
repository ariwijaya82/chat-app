import React from 'react'

import SearchIcon from '../assets/search.svg';


export default function Layout() {
  const [quiks, setQuiks] = React.useState<'close' | 'open' | 'inbox' | 'task'>('close');

  return (
    <div className="layout-page" onClick={() => {
      if (quiks === 'inbox' || quiks === 'task') setQuiks('open');
      else if (quiks === 'open') setQuiks('close');
    }}>
      <div className="sidebar"></div>
      <div className="content">
        <div className="searchbar">
          <img src={SearchIcon} alt="search-icon" />
          <input type="text" />
        </div>
        
      </div>
    </div>
  )
}