import './style.scss';

interface Props {
  text: string;
}

export default function LoadingSpinner({ text } : Props) {
  return (
    <div className="spinner-container">
      <div className="loading-spinner"></div>
      <div className="loading-text">{text}</div>
    </div>
  )
}