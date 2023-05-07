interface Props {
  text: string;
}

export default function Loading({ text } : Props) {
  return (
    <div className="loading">
      <div className="circle"></div>
      <div className="text">{text}</div>
    </div>
  )
}