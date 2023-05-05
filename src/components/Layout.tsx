import React from 'react'

import SearchIcon from '../assets/search.svg';
import QuicksIcon from '../assets/quicks.svg';
import InboxIcon from '../assets/inbox.svg';
import TaskIcon from '../assets/task.svg';

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
        <img
          className={`quicks-icon ${quiks}`}
          src={QuicksIcon}
          alt="quicks-icon"
          onClick={(e) => {
            e.stopPropagation();
            setQuiks(quiks === 'open' ? 'close' : 'open')
          }}
        />
        <p className={`icon-title inbox ${quiks}`}>Inbox</p>
        <img
          className={`inbox-icon ${quiks}`}
          src={InboxIcon}
          alt="inbox-icon"
          onClick={(e) => {
            e.stopPropagation();
            setQuiks('inbox')
          }}
        />
        <p className={`icon-title task ${quiks}`}>Task</p>
        <img
          className={`task-icon ${quiks}`}
          src={TaskIcon}
          alt="task-icon"
          onClick={(e) => {
            e.stopPropagation();
            setQuiks('task')
          }}
        />
      </div>
    </div>
  )
}