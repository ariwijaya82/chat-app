import SearchIcon from '../assets/search.svg';
import QuicksIcon from '../assets/quicks-icon.svg';

export default function Layout() {
  return (
    <div className="layout-page">
      <div className="sidebar"></div>
      <div className="content">
        <div className="searchbar">
          <img src={SearchIcon} alt="search-icon" />
          <input type="text" />
        </div>
        <img className="quicks-icon" src={QuicksIcon} alt="quicks-icon" />
      </div>
    </div>
  )
}